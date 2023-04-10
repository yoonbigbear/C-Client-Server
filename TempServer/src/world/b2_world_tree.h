#pragma once

#include "pre.h"
#include "box2d/b2_dynamic_tree.h"

#define TOPROXYPTR(proxy) Proxy* proxy_ptr = reinterpret_cast<Proxy*>(proxy);

struct Proxy
{
public:
    std::uint32_t eid;
    int32_t proxy_id;
    std::shared_ptr<b2Shape> b2shape;
};

class BroadPhaseBox
{
public:
    bool QueryCallback(int32_t id)
    {
        collisions.push_back(id);
        return true;
    }
    Vector<int32_t> collisions;
};

class b2WorldTree
{
public:
    void Initialize(const b2AABB& boundary);
    bool Spawn(const Vec& pos, float radius, Proxy* entity_data);
    void Despawn(const Proxy& proxy_data);
    bool Move(const Vec& pos, const Proxy& proxy_data);
    Set<Entity> Query(const Vec& pos, float range, Entity except = entt::null);

private:

    bool Contains(const b2AABB& box)
    {
        return boundary_.Contains(box);
    }

    bool Contains([[maybe_unused]] const Vec& pos)
    {
        //navigation
        return true;
    }


private:
    b2AABB boundary_;
    b2DynamicTree broad_phase_;
};