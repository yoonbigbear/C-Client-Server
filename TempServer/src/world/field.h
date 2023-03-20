#pragma once

#include "pre.h"
#include "box2d/b2_dynamic_tree.h"

#include <share/detour/DetourNavMesh.h>

#define ENTITYDATA(proxy) EntityData* entitydata = reinterpret_cast<EntityData*>(proxy);

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

class Field
{
public:
    void Initialize(const AABB2& boundary);
    bool Spawn(const Vec& pos, const AABB2& box,
        void* entity_data, int32_t& out_proxy);

    Vector<void*> Query(const Vec& pos, float range);

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