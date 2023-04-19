#pragma once

#include "pre.h"

void Recv_MoveNfy(void* session, Vector<uint8_t>& data);
void Recv_MoveAck(void* session, Vector<uint8_t>& data);

struct Mover
{
    Vec dest;
    Vec dir;
};

class MoveSystem
{
public:
    static void Send_MoveReq(class NetTcp* net, const Vec3& dst);
    static void MoveAlongPath(class Region& region, float dt);
};