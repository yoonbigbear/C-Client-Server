#pragma once

#include "pre.h"

#include "factory_system.h"
#include "schedule_system.h"
#include "debug_system.h"

void Recv_ChatNfy(void* session, Vector<uint8_t>& data);
void Recv_UpdateNeighborsNfy(void* session, Vector<uint8_t>& data);
void Recv_EnterNeighborsNfy(void* session, Vector<uint8_t>& data);
void Recv_LeaveNeighborsNfy(void* session, Vector<uint8_t>& data);
void Recv_MoveNfy(void* session, Vector<uint8_t>& data);
void Recv_MoveAck(void* session, Vector<uint8_t>& data);
void Recv_EnterWorldAck(void* session, Vector<uint8_t>& data);

void MoveAlongPath(Shared<class Scene> world, float dt);
void Send_MoveReq(class NetTcp* net, const Vec3& dst);

void Recv_Debug3DPosition(void* session, Vector<uint8_t>& data);