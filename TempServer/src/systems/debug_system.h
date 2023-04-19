#pragma once
#include "pre.h"

void Recv_DebugColliderReq(void* session, Vector<uint8_t>& data);

class DebugSystem
{
public:
    static void CreateDebugColliderObject(class Region& region, 
        Entity eid, short dir, uint8_t type);

    static struct Collider CreateCollider(short dir, const Vec& center, uint8_t type);

private:
    static Unique<struct DebugColliderInfoT> SerializeDebugCollider(const Vec& center, uint8_t type, short dir);
};