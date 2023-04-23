#pragma once
#include "pre.h"

void Recv_EnterWorldReq(void* session, Vector<uint8_t>& data);
void Recv_MoveReq(void* session, Vector<uint8_t>& data);
void Recv_DashReq(void* session, Vector<uint8_t>& data);

class MoveSystem
{
public:
    static bool DashTo(class Region& region, Vec& end, Entity eid);
    static bool MoveTo(class Region& region, Vec& end, Entity eid);
};