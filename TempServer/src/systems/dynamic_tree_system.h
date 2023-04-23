#pragma once

#include "pre.h"

class DynamicTreeSystem
{
public:
    static bool Spawn(class Region& region, const Vec& pos, float radius, struct b2Proxy* entity_data);
    static Set<Entity> Contact(class Region& region, const Vec& next, float range, Entity eid);
};