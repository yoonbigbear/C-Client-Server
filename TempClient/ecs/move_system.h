#pragma once

#include "pre.h"

void Recv_MoveNfy(void* session, Vector<uint8_t>& data);
void Recv_MoveAck(void* session, Vector<uint8_t>& data);

void Recv_DashAck(void* session, Vector<uint8_t>& data);
void Recv_DashNfy(void* session, Vector<uint8_t>& data);

struct Mover
{
    Vec dest;
    Vec dir;
};

class MoveSystem
{
public:
    static void Send_MoveReq(class NetTcp* net, const Vec3& dst);
    static void Send_DashReq(class NetTcp* net, const short dir);
    static void Update(class Region& region, float dt);

    static void MoveTo(class Region& region, Entity eid, Vec& end, float spd);
};