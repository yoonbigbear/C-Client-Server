#pragma once

#include "pre.h"
#include "share/navigation.h"

class Navigation_C : public Navigation
{
public:
    Navigation_C() : Navigation() {}
    virtual ~Navigation_C() {}

    bool ScreenRay(float* src, float* dst, float& tmin);
};