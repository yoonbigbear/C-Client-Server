#include "world.h"

#include "components.h"
#include <systems/systems.h>

#include "fbb/chat_generated.h"
#include "fbb/packets_generated.h"
#include "fbb/world_generated.h"

void World::Initialize()
{
    field_.Initialize(AABB2(vec2(-100, -100), vec2(100, 100)));

    SpawnAI();
}

void World::Enter(Shared<ClientSession> session)
{
    auto entity = registry_.create();

    registry_.emplace<NetComponent>(entity, session);

    //start pos
    auto& tf = registry_.emplace<PositionComponent>(entity);
    tf.v.v3.Set(RandomGenerator::Real(-50, 50), RandomGenerator::Real(-50, 50), 0);

    // entity info
    auto& entity_data = registry_.emplace<EntityData>(entity);
    entity_data.eid = (std::uint32_t)entity;
    entity_data.flag = EntityFlag::Player;
    entity_data.tid = 1;

    //box collider
    auto& box_comp = registry_.emplace<AABBComponent>(entity);
    auto half_box = vec2(5.f, 5.f);
    box_comp.box.lowerBound = tf.v.v2 - half_box;
    box_comp.box.upperBound = tf.v.v2 + half_box;

    //field
    field_.Spawn(tf.v.v2, box_comp.box, (void*)&entity_data, entity_data.proxy);
    
    //sight
    auto& sight_comp = registry_.emplace<SightComponent>(entity);
    sight_comp.range = 500;

    //sync
    auto entities = field_.Query(tf.v.v2, sight_comp.range);
    BroadcastSightSync(registry_, entities, session,
        static_cast<std::uint32_t>(entity));

    LOG_INFO("Enter World {}", static_cast<std::uint32_t>(entity));
}

void World::Enter(int npcid)
{
    auto entity = registry_.create();

    // entity info
    auto& entity_data = registry_.emplace<EntityData>(entity);
    entity_data.eid = (std::uint32_t)entity;
    entity_data.flag = EntityFlag::Player;
    entity_data.tid = 1;

    // pos
    auto& tf = registry_.emplace<PositionComponent>(entity);
    tf.v.v3.Set(RandomGenerator::Real(-50, 50), RandomGenerator::Real(-50, 50), 0);
    
    // collider
    auto& box_comp = registry_.emplace<AABBComponent>(entity);
    auto half_box = vec2(5.f, 5.f);
    box_comp.box.lowerBound = tf.v.v2 - half_box;
    box_comp.box.upperBound = tf.v.v2 + half_box;

    //field
    field_.Spawn(tf.v.v2, box_comp.box, (void*)&entity_data, entity_data.proxy);

    registry_.emplace<NpcComponent>(entity, npcid);
    {
        auto& wander = registry_.emplace<WanderComponent>(entity);
        wander.range = 20;
        wander.spawn_pos = tf.v;
    }

    auto& sight_comp = registry_.emplace<SightComponent>(entity);
    sight_comp.range = 500.f;

    LOG_INFO("npc enter world {}", static_cast<std::uint32_t>(entity));

}

void World::Update(float dt)
{
    //ai
    Wander(registry_, field_, dt);
    Move(registry_, dt);

    //Broadcast(BroadcastChat("broadcast from server"));
}

void World::SpawnAI()
{
    int aicount = 12;
    for (int i = 0; i < aicount; ++i)
    {
        Enter(1);
    }
}

void World::Broadcast(Vector<uint8_t>&& data)
{
    auto view = registry_.view<NetComponent>();
    for (auto [entity, net] : view.each())
    {
        net.session->Send(data);
    }
}