#pragma once

#include "pre.h"
#include "fbb/common_generated.h"

enum MoveFlag : uint8_t
{
    Start,
    Arrive,
    Moving,
};
struct Transform
{
    Vec v;
    short degree;
    float speed;
};

struct Mover
{
    Vec dest;
    Vec dir;
};

struct Neighbor
{
    Set<entt::entity> neighbors;
};

struct NpcComponent
{
    int id;
};

struct WanderComponent
{
    float acc_time;
    float range;
    Vec spawn_pos;
};

struct PathList
{
    Deque<Vec> paths;
    MoveFlag flag;
};
