#pragma once

#include "pre.h"


struct Transform
{
    Vec v;
    short degree;
    float speed;
    float base_spd;
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