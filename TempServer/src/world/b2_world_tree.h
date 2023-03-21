#pragma once

#include "pre.h"
#include "box2d/b2_dynamic_tree.h"

#define PROXY(proxy) Proxy* proxy_ptr = reinterpret_cast<Proxy*>(proxy);

struct Proxy
{
    uint8_t flag;
    std::uint32_t eid;
    int32_t proxy_id;
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
    void Initialize(const AABB2& boundary);
    bool Spawn(const Vec& pos, const AABB2& box,
        Proxy* entity_data);

    Vector<Proxy*> Query(const Vec& pos, float range, uint32_t except);

private:

    bool Contains(const AABB2& box)
    {
        return boundary_.Contains(box);
    }

    bool Contains(const Vec& pos)
    {
        //navigation
        return true;
    }


private:
   AABB2 boundary_;
   b2DynamicTree broad_phase_;
   
};