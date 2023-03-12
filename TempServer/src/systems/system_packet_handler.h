#pragma once

#include <entt/entt.hpp>
#include "components.h"

#include "fbb/world_generated.h"
#include "fbb/chat_generated.h"
#include "fbb/packets_generated.h"

static void ChatREQ(std::vector<uint8_t>& data)
{
    auto req = flatbuffers::GetRoot<ChatReq>(data.data());
    printf(req->chat()->c_str());
}

static void BroadcastSightSync(const entt::registry& registry,
    const Vector<void*>& entities,
    Weak<ClientSession> session, uint32_t except)
{
    flatbuffers::FlatBufferBuilder fbb(256);
    SightSyncT sync;
    for (auto target : entities)
    {
        EntityData* data = (EntityData*)target;
        if (data->eid != except)
        {
            auto tf = registry.get<const PositionComponent>((entt::entity)data->eid);
            sync.entities.emplace_back(EntityInfo{
                VecToXYZ<Vec3>(tf.v),
                data->flag,
                data->tid,
                data->eid });
        }
    }
    fbb.Finish(SightSync::Pack(fbb, &sync));

    if (auto ptr = session.lock(); ptr)
        ptr->Send((uint16_t)PacketId::Sight_Sync, fbb.GetSize(), fbb.GetBufferPointer());
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

static void SyncMove(const MoveComponent& mover,
    Weak<ClientSession> session,
    uint32_t eid)
{
    flatbuffers::FlatBufferBuilder fbb(256);
    MoveSyncT sync;
    sync.dest = std::make_unique<Vec3>(VecToXYZ<Vec3>(mover.dest));
    sync.spd = mover.speed;
    sync.dir = std::make_unique<Vec3>(VecToXYZ<Vec3>(mover.dir));
    sync.eid = eid;
    fbb.Finish(MoveSync::Pack(fbb, &sync));
    if (auto ptr = session.lock(); ptr)
        ptr->Send((uint16_t)PacketId::Move_Sync, fbb.GetSize(), fbb.GetBufferPointer());
}
