#pragma once
#include "pre.h"

void Recv_EnterWorldReq(void* session, Vector<uint8_t>& data);
void Recv_MoveReq(void* session, Vector<uint8_t>& data);
void Recv_DashReq(void* session, Vector<uint8_t>& data);

struct Mover
{
    Vec dest;
    Vec dir;
};

enum MoveFlag : uint8_t
{
    Start,
    Arrive,
    Moving,
};

struct PathList
{
    Deque<Vec> paths;
    MoveFlag flag;
};

class MoveSystem
{
public:
    static bool DashTo(class Region& region, Vec& end, Entity eid);
    static bool MoveTo(class Region& region, Vec& end, Entity eid, float spd);
    static bool PathTo(class Region& region, const Vec& end, Entity eid);

    static void UpdatePath(class Region& region);
    static void UpdateMove(class Region& region, float dt);
};