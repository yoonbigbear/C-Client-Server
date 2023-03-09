#pragma once

#include "pre.h"

#include "fbb/chat_generated.h"
#include "fbb/packets_generated.h"



b2Vec2 RandomPointInCircle2(float range, int max_try = 10)
{
    float x;
    float y;
    for (int i = 0; i < max_try; ++i)
    {
        x = RandomGenerator::Real(-1.f, 1.f);
        y = RandomGenerator::Real(-1.f, 1.f);
        if (x * x + y * y <= 1)
        {
            return range * b2Vec2(x, y);
        }
    }
    return b2Vec2(0,0);
}


static void ChatREQ(std::vector<uint8_t>& data)
{
    auto req = flatbuffers::GetRoot<ChatReq>(data.data());
    printf(req->chat()->c_str());
}

static std::vector<uint8_t> BroadcastChat(const std::string& str)
{
    flatbuffers::FlatBufferBuilder fbb(256);
    ChatSyncT sync;
    sync.chat = str;
    fbb.Finish(ChatSync::Pack(fbb, &sync));

    PacketHeader header;
    header.id = (short)PacketId::Chat_Sync;
    header.size = fbb.GetSize();
    std::vector<uint8_t> data(sizeof(PacketHeader) + fbb.GetSize());
    memcpy(data.data(), &header, sizeof(PacketHeader));
    memcpy(data.data() + sizeof(PacketHeader), fbb.GetBufferPointer(), fbb.GetSize());
    return data;
}

static void PacketHandling(entt::registry& registry, PacketHandler& packet_handler)
{
    auto view = registry.view<NetComponent>();
    for (auto [entity, net] : view.each())
    {
        auto list = net.session->recv_buffer().Get();
        for (auto& e : list)
        {
            packet_handler.Invoke(e.first)(e.second);
        }
    }
}

static void Wander(entt::registry& registry, float dt)
{
    auto view = registry.view<WanderComponent, PositionComponent>(entt::exclude<MoveComponent>);
    for (auto [entity, wandering, position] : view.each())
    {
        if (wandering.acc_time -= dt > 0.f)
        {
            continue;
        }
        else
        {
            auto next_end = RandomPointInCircle2(wandering.range) + wandering.spawn_pos;
            auto& mover = registry.emplace<MoveComponent>(entity);
            mover.end_pos = next_end;
            mover.speed = 10;
        }
    }
}

static void Move(entt::registry& registry, float dt)
{
    auto view = registry.view<const MoveComponent, PositionComponent>();
    for (auto [entity, mover, position] : view.each())
    {
        //auto len = position.pos.Len(mover.end_pos);
        //auto dir = position.pos.GetDir(mover.end_pos);
        auto dist = mover.end_pos - position.pos;
        auto len = dist.Normalize();

        if (len < dt * mover.speed)
        {
            position.pos = mover.end_pos;

            if (registry.all_of<WanderComponent>(entity))
            {
                registry.get<WanderComponent>(entity).acc_time = 2;
                registry.remove<MoveComponent>(entity);
            }
        }
        else
        {
            auto mv = dt * mover.speed * dist;
            position.pos += mv;
        }
    }
}

