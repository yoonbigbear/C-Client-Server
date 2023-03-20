#include "world.h"

#include "components.h"
#include <systems/systems.h>

#include "field.h"
#include "share/navigation.h"

#include "fbb/chat_generated.h"
#include "fbb/packets_generated.h"
#include "fbb/world_generated.h"

void World::Initialize()
{
    navigation_ = std::make_shared<Navigation>();
    field_ = std::make_shared<Field>();

    navigation_->Initialize("../Resource/all_tiles_navmesh.bin");
    field_->Initialize(AABB2(Vec2(-100, -100), Vec2(100, 100)));

    SpawnAI();
}

void World::Enter(Shared<ClientSession> session)
{
    auto entity = create();

    emplace<NetComponent>(entity, session);

    //start pos
    auto& tf = emplace<Transform>(entity);
    tf.v.v3.Set(RandomGenerator::Real(-50, 50), RandomGenerator::Real(-50, 50), 0);
    tf.angle = static_cast<short>(std::atan2f(tf.v.v2.y, tf.v.v2.x));

    // entity info
    auto& entity_data = emplace<EntityData>(entity);
    entity_data.eid = (std::uint32_t)entity;
    entity_data.flag = (uint8_t)EntityFlag::Player;
    entity_data.tid = 1;

    //box collider
    auto& box_comp = emplace<AABBComponent>(entity);
    auto half_box = Vec2(5.f, 5.f);
    box_comp.box.lowerBound = tf.v.v2 - half_box;
    box_comp.box.upperBound = tf.v.v2 + half_box;

    //field
    field_->Spawn(tf.v.v2, box_comp.box, (void*)&entity_data, entity_data.proxy);
    
    //sight
    auto& sight_comp = emplace<SightComponent>(entity);
    sight_comp.range = 50;

    //query sight entities
    auto entities_in_sight = field_->Query(tf.v.v2, sight_comp.range);
    Vector<EntityData*> datas;
    for (auto e : entities_in_sight)
    {
        ENTITYDATA(e);
        datas.emplace_back(entitydata);
    }
    std::sort(datas.begin(), datas.end());

    //EnterResp
    {
        flatbuffers::FlatBufferBuilder fbb(64);
        EnterWorldRespT resp;

        {
            fbVec pos = VecTo<fbVec>(tf.v.v3);
            fbVec endpos = pos;
            float speed = 0.f;
            if (all_of<Mover>(entity))
            {
                auto mover = get<Mover>(entity);
                endpos = VecTo<fbVec>(mover.dest);
                speed = mover.speed;
            }
            auto sender_info = EntityInfo{
                   pos, endpos, speed,
                   0,(uint32_t)entity,
                   tf.angle, EntityFlag::Player };
            resp.entity = std::make_unique<EntityInfo>(
                pos, endpos, speed, 0, (uint32_t)entity, tf.angle,
                EntityFlag::Player);
        }
        for (auto e : datas)
        {
            _ASSERT(valid((entt::entity)e->eid));
            if (all_of<Transform>((entt::entity)e->eid))
            {
                auto target_tf = get<Transform>((entt::entity)e->eid);
                fbVec pos = VecTo<fbVec>(tf.v.v3);
                fbVec endpos = pos;
                float speed = 0.f;
                
                if (all_of<Mover>((entt::entity)e->eid))
                {
                    auto mover = get<Mover>((entt::entity)e->eid);
                    endpos = VecTo<fbVec>(mover.dest);
                    speed = mover.speed;
                }
                resp.sight_entities.emplace_back(EntityInfo{
                     pos, endpos, speed, 0, (uint32_t)e->eid, target_tf.angle,
                        (EntityFlag)e->flag });
            }
        }
        
        fbb.Finish(EnterWorldResp::Pack(fbb, &resp));
        session->Send((uint16_t)PacketId::EnterWorld_Resp, fbb.GetSize(), fbb.GetBufferPointer());

        LOG_INFO("[SERVER] send EnterWorldResp: ");
    }

    SightSyncronize(shared(), (uint32_t)entity);

    LOG_INFO("Enter World {}", static_cast<std::uint32_t>(entity));
}

void World::Enter(int npcid)
{
    auto entity = create();

    // entity info
    auto& entity_data = emplace<EntityData>(entity);
    entity_data.eid = (std::uint32_t)entity;
    entity_data.flag = (uint8_t)EntityFlag::Player;
    entity_data.tid = 1;

    // pos
    auto& tf = emplace<Transform>(entity);
    tf.v.v3.Set(RandomGenerator::Real(-50, 50), RandomGenerator::Real(-50, 50), 0);
    
    // collider
    auto& box_comp = emplace<AABBComponent>(entity);
    auto half_box = Vec2(5.f, 5.f);
    box_comp.box.lowerBound = tf.v.v2 - half_box;
    box_comp.box.upperBound = tf.v.v2 + half_box;

    //field
    field_->Spawn(tf.v.v2, box_comp.box, (void*)&entity_data, entity_data.proxy);

    emplace<NpcComponent>(entity, npcid);
    {
        auto& wander = emplace<WanderComponent>(entity);
        wander.range = 20;
        wander.spawn_pos = tf.v;
        wander.acc_time = 5;
    }

    auto& sight_comp = emplace<SightComponent>(entity);
    sight_comp.range = 50.f;

    LOG_INFO("npc enter world {}", static_cast<std::uint32_t>(entity));

}

void World::Update(float dt)
{
    MoveAlongPath(shared());

    //ai
    Wander(shared(), dt);
    Move(shared(), dt);


    //Broadcast(BroadcastChat("broadcast from server"));
}

void World::SpawnAI()
{
    int aicount = 50;
    for (int i = 0; i < aicount; ++i)
    {
        Enter(1);
    }
}

void World::Broadcast(Vector<uint8_t>&& data)
{
    auto v = view<NetComponent>();
    for (auto&& [entity, net] : v.each())
    {
        net.session->Send(data);
    }
}

bool World::HandleMove(uint32_t eid, const Vec& dest)
{
    ENTITY(eid);
    if (valid(entity))
    {
        auto tf = try_get<Transform>(entity);
        if (tf)
        {
            auto& mover = emplace_or_replace<Mover>(entity);
            mover.dest = dest;

            
            mover.dir = tf->v.v2 - mover.dest.v2;
            mover.dir.v2.Normalize();

            mover.speed = 1.f;
        }
    }
    return false;
}