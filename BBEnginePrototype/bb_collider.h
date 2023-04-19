#pragma once

#include "types.h"

enum ColliderType : uint8_t
{
    None,
    Circle,
    Sector,
    AABB,
    OBB,
    Ray,
};

struct CircleInfo
{
    float r;
};

struct SectorInfo
{
    short dir;
    short half_angle;
    float r;
};
struct AABBInfo
{
    float hx;
    float hy;
};
struct OBBInfo
{
    short dir;
    float hw;
    float hl;
};
struct RayInfo
{
    short dir;
    float length;
};

struct Collider
{
    Vec center;
    ColliderType type;
    union
    {
        CircleInfo circle;
        SectorInfo sector;
        AABBInfo aabb;
        OBBInfo obb;
        RayInfo ray;
    };
};
