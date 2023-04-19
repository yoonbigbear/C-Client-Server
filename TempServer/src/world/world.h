#pragma once

#include "pre.h"
#include "region.h"

class World
{
public:
    void Start()
    {
        for (int i = 0; i < 16; ++i)
        {
            Region region;
            regions.emplace_back();
        }
    }

private:
    std::vector<Region> regions;
};