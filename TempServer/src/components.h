#pragma once

#include "net/client_session.h"
#include "fbb/common_generated.h"

#include "box2d/b2_collision.h"

struct NetComponent
{
    std::shared_ptr<ClientSession> session;
};

struct PositionComponent
{
    b2Vec2 pos;
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
    std::vector<std::uint32_t> entities;
};

struct NpcComponent
{
    int id;
};

struct WanderComponent
{
    float acc_time;
    float range;
    b2Vec2 spawn_pos;
};

struct MoveComponent
{
    b2Vec2 end_pos;
    float speed;
};