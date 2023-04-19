#pragma once
#include "dynamic_tree_system.h"

#include "world/b2_world_tree.h"
#include "world/region.h"

bool DynamicTreeSystem::Spawn(b2WorldTree& tree, const Vec& pos, float radius, Proxy* entity_data)
{
    return tree.Spawn(pos, radius, entity_data);
}

void DynamicTreeSystem::Contact(Region& region, const Vec& next, float range, Entity eid)
{
    if (region.Valid<Transform, Proxy>(eid))
    {
        auto collisions = region.dyanmic_tree()->Query(next, range, eid);
        for (auto& e : collisions)
        {
            if (region.Valid<Proxy>(e))
            {
                auto& proxy = region.get<Proxy>(e);
                proxy.eid;
            }
        }
    }
}
