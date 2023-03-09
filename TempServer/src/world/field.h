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

    bool Spawn(const vec2& pos, 
        const AABB2& box,
        void* entity_data)
    {
        if (Checkboundary(box) &&
            ValidPos(pos))
        {
            auto proxy = broad_phase_.CreateProxy(box, entity_data);
            return true;
        }
        return false;
    }

    auto Query(const AABB2& sight)
    {
        BroadPhaseBox collisions;
        broad_phase_.Query<BroadPhaseBox>(&collisions, sight);
        std::vector<void*> targets;
        for (auto& target : collisions.collisions)
        {
            targets.push_back(broad_phase_.GetUserData(target));
        }
        return targets;
    }

private:

    bool Checkboundary(const AABB2& box)
    {
        return boundary_.Contains(box);
    }

    bool ValidPos(const vec2& pos)
    {
        //navigation
        return true;
    }


private:
   AABB2 boundary_;
   b2DynamicTree broad_phase_;
};