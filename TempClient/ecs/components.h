#pragma once

#include "types.h"

struct Transform
{
    vec v;
};

struct MoveInfo
{
    vec dest;
    vec dir;
    float speed;
};

struct CylinderData
{
    enum
    {
        max_climb = 1,
    };
    float height;
    float radius;
};

struct EntityData
{
    uint32_t eid;
};