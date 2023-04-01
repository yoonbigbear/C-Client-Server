#include "region.h"

#include "components.h"
#include "systems/systems.h"
#include "net/user.h"
#include "packet_handler.h"
#include "share/navigation.h"
#include "b2_world_tree.h"

#include "fbb/chat_generated.h"
#include "fbb/packets_generated.h"
#include "fbb/world_generated.h"

bool Region::Initialize()
{
    navigation_ = std::make_shared<Navigation>();
    world_tree_ = std::make_shared<b2WorldTree>();

    if (!navigation_->Initialize("all_tiles_navmesh.bin"))
        return false;
    world_tree_->Initialize(AABB2(Vec2(-100, -100), Vec2(100, 100)));

    viewing_range_ = 30;

    SpawnAI();

    return true;
}

entt::entity Region::EnterPlayer(Shared<User> user)
{
    auto entity = create();

    if (user)
    {
        emplace<NetComponent>(entity, user);
    }

    //start pos
    auto& tf = emplace<Transform>(entity);
    tf.v.v3.Set(RandomGenerator::Real(-50, 50), RandomGenerator::Real(-50, 50), 0);
    tf.degree = static_cast<short>(std::atan2f(tf.v.v2.y, tf.v.v2.x));

    // entity info
    auto& proxy_data = emplace<Proxy>(entity);
    proxy_data.eid = (std::uint32_t)entity;

    //field
    if (!world_tree_->Spawn(tf.v.v2, 5.f, &proxy_data))
    {
        destroy(entity);
        LOG_ERROR("b2tree create proxy failed");
        return entt::null;
    }

    //query sight entities
    emplace<SightComponent>(entity);
    auto proxies_on_sight = world_tree_->Query(tf.v.v2, viewing_range(), entity);

    //send the entites info on sight to the created player
    Send_EnterNeighborsResp(*this, entity);

    //send created player's enter packet to neighbors
    UpdateNeighbors(*this, proxies_on_sight, entity);

    user->world(shared());

    LOG_INFO("Enter World {}", static_cast<std::uint32_t>(entity));

    return entity;
}

entt::entity Region::Enter(int npcid)
{
    auto entity = create();


    // pos
    auto& tf = emplace<Transform>(entity);
    tf.v.v3.Set(RandomGenerator::Real(-50, 50), RandomGenerator::Real(-50, 50), 0);

    // collider
    auto& proxy_data = emplace<Proxy>(entity);
    proxy_data.eid = (std::uint32_t)entity;
    
    //field
    if (!world_tree_->Spawn(tf.v.v2, 5.f, &proxy_data))
    {
        release(entity);
        return entt::null;
    }

    emplace<NpcComponent>(entity, npcid);
    {
        auto& wander = emplace<WanderComponent>(entity);
        wander.range = 20;
        wander.spawn_pos = tf.v;
        wander.acc_time = 5;
    }

    //query sight entities
    emplace<SightComponent>(entity);
    auto proxies_on_sight = world_tree_->Query(tf.v.v2, viewing_range(), entity);

    //update sight
    UpdateNeighbors(*this, proxies_on_sight, entity);

    LOG_INFO("npc enter world {}", static_cast<std::uint32_t>(entity));

    return entity;
}

void Region::Leave(entt::entity eid)
{
    auto& proxydata = get<Proxy>(eid);
    world_tree_->Despawn(proxydata);
    destroy(eid);
}

void Region::Update(float dt)
{
    //release packet
    {
        auto players = view<NetComponent>();
        for (auto [entity, net] : players.each())
        {
            net.user->ReadPackets();
        }
    }


    //ai
    Wander(shared(), dt);

    //move
    MoveAlongPath(*this);
    UpdateMove(*this, dt);

    //Broadcast(BroadcastChat("broadcast from server"));
}

void Region::SpawnAI()
{
    int aicount = 10;
    for (int i = 0; i < aicount; ++i)
    {
        Enter(1);
    }
}

void Region::Broadcast(Vector<uint8_t>&& data)
{
    auto v = view<NetComponent>();
    for (auto&& [entity, net] : v.each())
    {
        net.user->tcp()->Send(data);
    }
}

bool Region::HandleMove(uint32_t eid, const Vec& dest)
{
    TOENTITY(eid);
    if (valid(entity))
    {
        auto tf = try_get<Transform>(entity);
        if (tf)
        {
            /*auto& mover = emplace_or_replace<Mover>(entity);
            mover.dest = dest;

            
            mover.dir = tf->v.v2 - mover.dest.v2;
            mover.dir.v2.Normalize();*/

            auto& pathlist = emplace_or_replace<PathList>(entity);
            pathlist.paths.emplace_back(dest);

            tf->speed = 1.f;
            
            return true;
        }
    }
    return false;
}