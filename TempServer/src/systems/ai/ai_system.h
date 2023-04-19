#pragma once

#include "pre.h"

struct WanderComponent
{
    float acc_time;
    float range;
    Vec spawn_pos;
};

void Wander(Shared<class Region> world, float dt);
