#pragma once

#include "pre.h"
#include "net/client_session.h"
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
};

struct AABBComponent
{
    AABB2 box;
};

struct Mover
{
    Vec dest;
    Vec dir;
    float speed;
};

struct SightComponent
{
    Vector<struct Proxy*> objects;
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
    List<Vec> paths;
    MoveFlag flag;
};

struct NetComponent
{
    Shared<ClientSession> session;
};

