#pragma once

#include "pre.h"
#include "region.h"

class World
{
public:
    void AddRegion();

private:
    UnorderedMap<uint32_t, Region> regions;
};