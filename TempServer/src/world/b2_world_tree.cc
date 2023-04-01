#include "b2_world_tree.h"

#include <box2d/b2_circle_shape.h>
void b2WorldTree::Initialize(const AABB2& bound)
{
    boundary_ = bound;
}

bool b2WorldTree::Spawn(const Vec& pos, float radius, Proxy* entity_data)
{
    entity_data->b2shape = new b2CircleShape();
    b2AABB aabb;
    entity_data->b2shape->m_radius = radius;
    entity_data->b2shape->m_type = b2Shape::e_circle;
    auto tf = b2Transform(pos.v2, b2Rot(0));
    entity_data->b2shape->ComputeAABB(&aabb, tf, 0);
    if (Contains(aabb) && Contains(pos))
    {
        entity_data->proxy_id = broad_phase_.CreateProxy(aabb, entity_data);
        return true;
    }
    return false;
}

void b2WorldTree::Despawn(const Proxy& proxy_data)
{
    broad_phase_.DestroyProxy(proxy_data.proxy_id);
}

bool b2WorldTree::Move(const Vec& pos, const Proxy& proxy_data)
{
    b2AABB aabb;
    auto tf = b2Transform(pos.v2, b2Rot(0));
    proxy_data.b2shape->ComputeAABB(&aabb, tf, 0);
    return broad_phase_.MoveProxy(proxy_data.proxy_id, aabb, b2Vec2(0,0));
}

Set<entt::entity> b2WorldTree::Query(const Vec& pos, float range, entt::entity except)
{
    auto sight_half = range * 0.5f;
    auto sight = AABB2();
    sight.lowerBound.x = pos.v2.x - sight_half;
    sight.lowerBound.y = pos.v2.y - sight_half;
    sight.upperBound.x = pos.v2.x + sight_half;
    sight.upperBound.y = pos.v2.y + sight_half;

    BroadPhaseBox collisions;
    broad_phase_.Query<BroadPhaseBox>(&collisions, sight);
    Set<entt::entity> targets;

    for (auto& target : collisions.collisions)
    {
        TOPROXYPTR(broad_phase_.GetUserData(target));
        _ASSERT(proxy_ptr);
        TOENTITY(proxy_ptr->eid);
        if (entity != except)
            targets.insert(entity);
    }

    return targets;
}

Proxy::~Proxy()
{
    if (b2shape)
        delete b2shape;
}
