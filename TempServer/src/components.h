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


struct PathList
{
    Deque<Vec> paths;
    MoveFlag flag;
};
