#pragma once

#include "world/b2_world_tree.h"

class DynamicTreeSystem
{
public:
    static bool Spawn(b2WorldTree& tree, const Vec& pos, float radius, Proxy* entity_data)
    {
        return tree.Spawn(pos, radius, entity_data);
    }
};