#pragma once
#include "dynamic_tree_system.h"

#include "world/b2_world_tree.h"
#include "world/region.h"

bool DynamicTreeSystem::Spawn(Region& region, const Vec& pos, float radius, b2Proxy* entity_data)
{
    return region.dyanmic_tree()->Spawn(pos, radius, entity_data);
}

Set<Entity> DynamicTreeSystem::Contact(Region& region, const Vec& next, float range, Entity eid)
{
    Set<Entity> entities;
    if (region.Valid<Transform, b2Proxy>(eid))
    {
        auto collisions = region.dyanmic_tree()->Query(next, range, eid);
        for (auto& e : collisions)
        {
            if (region.Valid<b2Proxy>(e))
            {
                auto& proxy = region.get<b2Proxy>(e);
                entities.emplace(proxy.eid);
            }
        }
    }
    return entities;
}
