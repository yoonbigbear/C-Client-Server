#pragma once

#include <vector>
#include "box2d/b2_dynamic_tree.h"

class BroadPhaseBox
{
public:
    bool QueryCallback(int32_t id)
    {
        collisions.push_back(id);
        return true;
    }
    std::vector<int32_t> collisions;
};

class Field
{
public:

    bool Spawn(const b2Vec2& pos, 
        const b2AABB& box,
        void* entity_data)
    {
        if (Checkboundary(box) &&
            ValidPos(pos))
        {
            //auto proxy = broad_phase_.CreateProxy(box, entity_data);
            return true;
        }
        return false;
    }

    auto Query(const b2AABB& sight)
    {
        BroadPhaseBox collisions;
        //broad_phase_.Query<BroadPhaseBox>(&collisions, sight);
        std::vector<void*> targets;
        for (auto& target : collisions.collisions)
        {
            //targets.push_back(broad_phase_.GetUserData(target));
        }
        return targets;
    }

private:

    bool Checkboundary(const b2AABB& box)
    {
        return boundary_.Contains(box);
    }

    bool ValidPos(const b2Vec2& pos)
    {
        //navigation
        return true;
    }


private:
   b2AABB boundary_;
};