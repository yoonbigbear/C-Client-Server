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
    sight_comp.range = 500;

    //query sight entities
    auto entities_in_sight = field_->Query(tf.v.v2, sight_comp.range);
    Vector<EntityData*> datas;
    for (auto e : entities_in_sight)
    {
        ENTITYDATA(e);
        datas.emplace_back(entitydata);
    }
    std::sort(datas.begin(), datas.end());

    //send entity infos to me
    {
        flatbuffers::FlatBufferBuilder fbb(64);
        EnterWorldRespT resp;
        resp.entity = std::make_unique<EntityInfo>(
            VecTo<fbVec>(tf.v.v3),
            0,
            (uint32_t)entity,
            tf.angle,
            EntityFlag::Player);

        for (auto e : datas)
        {
            if (all_of<Transform>((entt::entity)e->eid))
            {
                auto target_tf = get<Transform>((entt::entity)e->eid);

                resp.sight_entities.emplace_back(EntityInfo{
                     VecTo<fbVec>(tf.v.v3),
                     0,
                    (uint32_t)e->eid,
                    target_tf.angle,
                        (EntityFlag)e->flag
                    });
            }
        }
        
        fbb.Finish(EnterWorldResp::Pack(fbb, &resp));
        session->Send((uint16_t)PacketId::EnterWorld_Resp, fbb.GetSize(), fbb.GetBufferPointer());

        LOG_INFO("[SERVER] send EnterWorldResp: ");
    }

    //send my data to new sight entities.
    {
        flatbuffers::FlatBufferBuilder fbb(256);
        EnterSyncT sync;
        sync.enter_entity = std::make_unique<EntityInfo>(EntityInfo{
                                      VecTo<fbVec>(tf.v.v3),//pos
                                      1,//tid
                                      (uint32_t)entity,//eid
                                      static_cast<int16_t>(tf.angle), //angle
                                      EntityFlag::Player });//flag
        fbb.Finish(EnterSync::Pack(fbb, &sync));
       
        
        Vector<EntityData*> new_list;
        std::set_difference(datas.begin(),
            datas.end(),
            sight_comp.objects.begin(),
            sight_comp.objects.end(), 
            std::inserter(new_list, new_list.begin()));
        for (auto data : new_list)
        {
            if (data->eid != (uint32_t)entity)
            {
                if (all_of<NetComponent>(entt::entity(data->eid)))
                {
                    auto net = get<NetComponent>(entt::entity(data->eid));
                    net.session->Send((uint16_t)PacketId::EnterSync,
                        fbb.GetSize(), fbb.GetBufferPointer());
                }
                sight_comp.objects.emplace_back(data);
            }
        }
    }

    //send leave to entities out of sight.
    {
        flatbuffers::FlatBufferBuilder fbb(256);
        LeaveSyncT sync;
        sync.leave_entity = ((uint32_t)entity);//flag
        fbb.Finish(LeaveSync::Pack(fbb, &sync));

        Vector<EntityData*> out_list;
        std::set_difference(sight_comp.objects.begin(), sight_comp.objects.end(),
            datas.begin(), datas.end(), 
            std::inserter(out_list, out_list.begin()));
        for (auto data : out_list)
        {
            if (data->eid != (uint32_t)entity)
            {
                if (all_of<NetComponent>(entt::entity(data->eid)))
                {
                    auto net = get<NetComponent>(entt::entity(data->eid));
                    net.session->Send((uint16_t)PacketId::LeaveSync,
                        fbb.GetSize(), fbb.GetBufferPointer());
                }
            }
        }
    }
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
    sight_comp.range = 500.f;

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
    int aicount = 10;
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

            mover.speed = 0.1;
        }
    }
    return false;
}