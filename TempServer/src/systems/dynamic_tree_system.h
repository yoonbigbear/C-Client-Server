#pragma once

#include "pre.h"

class DynamicTreeSystem
{
public:
    static bool Spawn(class b2WorldTree& tree, const Vec& pos, float radius, struct Proxy* entity_data);
    static void Contact(class Region& region, const Vec& next, float range, Entity eid);
};