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
    vec2 pos;
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
};

struct SightComponent
{
    float range;
    Vector<std::uint32_t> entities;
};

struct NpcComponent
{
    int id;
};

struct WanderComponent
{
    float acc_time;
    float range;
    vec2 spawn_pos;
};

struct MoveComponent
{
    vec2 end_pos;
    float speed;
};