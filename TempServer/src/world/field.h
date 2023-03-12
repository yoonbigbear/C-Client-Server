#pragma once

#include "pre.h"
#include "box2d/b2_dynamic_tree.h"

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
    bool Spawn(const vec2& pos, const AABB2& box, 
        void* entity_data, int32_t& out_proxy);


    Vector<void*> Query(const vec2& pos, float range);

private:

    bool Contains(const AABB2& box)
    {
        return boundary_.Contains(box);
    }

    bool Contains(const vec2& pos)
    {
        //navigation
        return true;
    }


private:
   AABB2 boundary_;
   b2DynamicTree broad_phase_;
};