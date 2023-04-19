#pragma once

#include "pre.h"


struct Transform
{
    Vec v;
    float speed;
    short degree;
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


struct PlayerSession
{
   class NetTcp* session;
};