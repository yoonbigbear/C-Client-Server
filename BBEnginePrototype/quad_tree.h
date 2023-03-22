#pragma once

#include "bb_math.h"

class Quadtree
{
public:
    bool Contains(float range, float center)
    {
        
    }

    //if viewing range 
    bool Contains(AABB2 viewing_range)
    {
        return boundary_.Contains(viewing_range);
    }

private:
    AABB2 boundary_;
};