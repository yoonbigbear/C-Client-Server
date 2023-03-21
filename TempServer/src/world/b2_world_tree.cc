#include "b2_world_tree.h"

void b2WorldTree::Initialize(const AABB2& bound)
{
    boundary_ = bound;
}

bool b2WorldTree::Spawn(const Vec& pos, const AABB2& box, Proxy* entity_data)
{
    if (Contains(box) && Contains(pos))
    {
        entity_data->proxy_id = broad_phase_.CreateProxy(box, entity_data);
        return true;
    }
    return false;
}

Vector<Proxy*> b2WorldTree::Query(const Vec& pos, float range, uint32_t except)
{
    auto sight_half = range * 0.5f;
    auto sight = AABB2();
    sight.lowerBound.x = pos.v2.x - sight_half;
    sight.lowerBound.y = pos.v2.y - sight_half;
    sight.upperBound.x = pos.v2.x + sight_half;
    sight.upperBound.y = pos.v2.y + sight_half;

    BroadPhaseBox collisions;
    broad_phase_.Query<BroadPhaseBox>(&collisions, sight);
    Vector<Proxy*> targets;

    for (auto& target : collisions.collisions)
    {
        PROXY(broad_phase_.GetUserData(target));
        if (proxy_ptr->eid != except)
            targets.emplace_back(proxy_ptr);
    }
    return targets;
}