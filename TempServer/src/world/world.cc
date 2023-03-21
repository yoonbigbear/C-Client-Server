#include "world.h"

#include "components.h"
#include "systems/systems.h"
#include "net/client_session.h"
#include "packet_handler.h"
#include "share/navigation.h"
#include "b2_world_tree.h"

#include "fbb/chat_generated.h"
#include "fbb/packets_generated.h"
#include "fbb/world_generated.h"

void World::Initialize()
{
    navigation_ = std::make_shared<Navigation>();
    field_ = std::make_shared<b2WorldTree>();

    navigation_->Initialize("../Resource/all_tiles_navmesh.bin");
    field_->Initialize(AABB2(Vec2(-100, -100), Vec2(100, 100)));

    viewing_range_ = 500;

    SpawnAI();
}

void World::Enter(Shared<ClientSession> session)
{
    auto entity = create();

    if (session)
    {
        emplace<NetComponent>(entity, session);
    }

    //start pos
    auto& tf = emplace<Transform>(entity);
    tf.v.v3.Set(RandomGenerator::Real(-50, 50), RandomGenerator::Real(-50, 50), 0);
    tf.degree = static_cast<short>(std::atan2f(tf.v.v2.y, tf.v.v2.x));

    // entity info
    auto& proxy_data = emplace<Proxy>(entity);
    proxy_data.eid = (std::uint32_t)entity;
    proxy_data.flag = (uint8_t)EntityFlag::Player;

    //box collider
    auto& box_comp = emplace<AABBComponent>(entity);
    auto half_box = Vec2(5.f, 5.f);
    box_comp.box.lowerBound = tf.v.v2 - half_box;
    box_comp.box.upperBound = tf.v.v2 + half_box;

    //field
    if (!field_->Spawn(tf.v.v2, box_comp.box, &proxy_data))
    {
        release(entity);
        return;
    }

    //query sight entities
    auto proxies_on_sight = field_->Query(tf.v.v2, viewing_range(), (uint32_t)entity);
    std::sort(proxies_on_sight.begin(), proxies_on_sight.end());

    //update sight
    SendEnterResp(shared(), proxies_on_sight, entity);
    SightSyncronize(shared(), proxies_on_sight, entity);

    LOG_INFO("Enter World {}", static_cast<std::uint32_t>(entity));
}

void World::Enter(int npcid)
{
    auto entity = create();

    // entity info
    auto& proxy_data = emplace<Proxy>(entity);
    proxy_data.eid = (std::uint32_t)entity;
    proxy_data.flag = (uint8_t)EntityFlag::Player;

    // pos
    auto& tf = emplace<Transform>(entity);
    tf.v.v3.Set(RandomGenerator::Real(-50, 50), RandomGenerator::Real(-50, 50), 0);
    
    // collider
    auto& box_comp = emplace<AABBComponent>(entity);
    auto half_box = Vec2(5.f, 5.f);
    box_comp.box.lowerBound = tf.v.v2 - half_box;
    box_comp.box.upperBound = tf.v.v2 + half_box;

    //field
    if (!field_->Spawn(tf.v.v2, box_comp.box, &proxy_data))
    {
        release(entity);
        return;
    }

    emplace<NpcComponent>(entity, npcid);
    {
        auto& wander = emplace<WanderComponent>(entity);
        wander.range = 20;
        wander.spawn_pos = tf.v;
        wander.acc_time = 5;
    }

    //query sight entities
    auto& sight_comp = emplace<SightComponent>(entity);
    auto proxies_on_sight = field_->Query(tf.v.v2, viewing_range(), (uint32_t)entity);
    std::sort(proxies_on_sight.begin(), proxies_on_sight.end());

    //update sight
    SightSyncronize(shared(), proxies_on_sight, entity);

    LOG_INFO("npc enter world {}", static_cast<std::uint32_t>(entity));
}

void World::Update(float dt)
{
    MoveAlongPath(shared());

    //ai
    Wander(shared(), dt);
    UpdateMove(shared(), dt);

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

            mover.speed = 1.f;
        }
    }
    return false;
}