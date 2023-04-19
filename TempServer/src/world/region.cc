#include "region.h"

#include "systems/systems.h"
#include "net/user.h"
#include "packet_handler.h"
#include "navigation.h"

#include "b2_world_tree.h"
#include "fbb/chat_generated.h"
#include "fbb/packets_generated.h"
#include "fbb/world_generated.h"

bool Region::Initialize(Vec pos)
{
    navigation_ = std::make_shared<Navigation>();
    dynamic_tree_ = std::make_shared<b2WorldTree>();

    if (!navigation_->Initialize("all_tiles_navmesh.bin"))
        return false;
    dynamic_tree_->Initialize(AABB2(Vec2(-100, -100), Vec2(100, 100)));

    viewing_range_ = 150;

    SpawnAI(0);

    on_destroy<Neighbor>().connect<UpdateDestroyed>();

    return true;
}

Entity Region::Enter(int npcid)
{
    auto entity = create();

    // pos
    auto& tf = emplace<Transform>(entity);
    tf.v.v3.Set(RandomGenerator::Real(-50, 50), RandomGenerator::Real(-50, 50), 0);

    // collider
    auto& proxy_data = emplace<Proxy>(entity);
    proxy_data.eid = (std::uint32_t)entity;
    
    //field
    if (!dynamic_tree_->Spawn(tf.v.v2, 5.f, &proxy_data))
    {
        LOG_ERROR("Failed entity spawn on the world tree x:{} y:{} z:{}", tf.v[0], tf.v[1], tf.v[2]);
        destroy(entity);
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
    emplace<Neighbor>(entity);
    auto proxies_on_sight = dynamic_tree_->Query(tf.v.v2, viewing_range(), entity);

    //update sight
    WorldSystem::UpdateNeighbors(*this, proxies_on_sight, entity);

    LOG_INFO("npc enter world {}", static_cast<std::uint32_t>(entity));

    return entity;
}

void Region::Leave(Entity eid)
{
    if (valid(eid))
    {
        auto& proxydata = get<Proxy>(eid);
        dynamic_tree_->Despawn(proxydata);
        destroy(eid);
    }
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

void Region::SpawnAI(int n)
{
    int aicount = n;
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

void Region::BroadcastInRange(const Vec& pos, float r, 
    const flatbuffers::FlatBufferBuilder& fbb, uint16_t id)
{
    auto list = dynamic_tree_->Query(pos, r);
    for (auto e : list)
    {
        Send(e, fbb, id);
    }
}

void Region::Send(const Entity eid, const flatbuffers::FlatBufferBuilder& fbb, uint16_t id) const
{
    if (Valid<NetComponent>(eid))
    {
        get<NetComponent>(eid).user->tcp()->Send(id, fbb.GetSize(), fbb.GetBufferPointer());
    }
}

bool Region::HandleMove(Entity entity, const Vec& dest)
{
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