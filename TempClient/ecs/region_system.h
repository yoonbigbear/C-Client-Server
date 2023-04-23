#pragma once
#include "pre.h"

void Recv_UpdateNeighborsNfy(void* session, Vector<uint8_t>& data);
void Recv_EnterNeighborsNfy(void* session, Vector<uint8_t>& data);
void Recv_LeaveNeighborsNfy(void* session, Vector<uint8_t>& data);
void Recv_EnterWorldAck(void* session, Vector<uint8_t>& data);

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

struct PathList
{
    Deque<Vec> paths;
    MoveFlag flag;
};

struct Neighbor
{
    Vector<EntityData*> neighbors;
};

class RegionSystem
{
public:
    static bool PathTo(class Region& region, Entity eid, Vec& start, Vec& end);
    static void Update(class Region& region);
};