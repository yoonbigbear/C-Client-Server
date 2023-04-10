#pragma once

#include "pre.h"


struct EntityData
{
    uint8_t flag;
    std::uint32_t eid;
    uint32_t tid;
    int32_t proxy;
};

enum MoveFlag : uint8_t
{
    Start,
    Arrive,
    Moving,
};
struct Transform
{
    Vec v;
    float speed;
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
};

struct Neighbor
{
    Vector<EntityData*> neighbors;
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

struct CylinderData
{
    enum
    {
        max_climb = 1,
    };
    float height;
    float radius;
};

struct PlayerSession
{
   class NetTcp* session;
};

struct TimerComponent
{
    float sec;
    Action<> action;
};

struct ColliderComponent
{
    Collider info;
};

