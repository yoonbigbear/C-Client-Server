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

void Region::Leave(Entity eid)
{
    if (valid(eid))
    {
        auto& proxydata = get<b2Proxy>(eid);
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
    MoveSystem::UpdatePath(*this);
    MoveSystem::UpdateMove(*this, dt);

    //Broadcast(BroadcastChat("broadcast from server"));
}

void Region::SpawnAI(int n)
{
    int aicount = n;
    for (int i = 0; i < aicount; ++i)
    {
        NpcSystem::CreateNpc(*this, 10000001);
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