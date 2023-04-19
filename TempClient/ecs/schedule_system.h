#pragma once

#include "pre.h"


struct TimerComponent
{
    float sec;
    Action<> action;
};

void UpdateTimer(Shared<class Region> scene, float dt);
