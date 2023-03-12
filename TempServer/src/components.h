#pragma once

#include "pre.h"
#include "net/client_session.h"
#include "fbb/common_generated.h"

struct NetComponent
{
    std::shared_ptr<ClientSession> session;
};

struct PositionComponent
{
    vec v;
};

struct AABBComponent
{
    b2AABB box;
};

struct EntityData
{
    EntityFlag flag;
    std::uint32_t eid;
    uint32_t tid;
    int32_t proxy;

};

struct SightComponent
{
    float range;
};

struct NpcComponent
{
    int id;
};

struct WanderComponent
{
    float acc_time;
    float range;
    vec spawn_pos;
};

struct MoveComponent
{
    vec dest;
    vec dir;
    float speed;
};