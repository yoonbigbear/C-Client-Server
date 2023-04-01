#pragma once

#include "pre.h"

#include "factory_system.h"

void Recv_ChatSync(void* session, Vector<uint8_t>& data);
void Recv_UpdateNeighborsSync(void* session, Vector<uint8_t>& data);
void Recv_EnterNeighborsSync(void* session, Vector<uint8_t>& data);
void Recv_LeaveNeighborsSync(void* session, Vector<uint8_t>& data);
void Recv_MoveSync(void* session, Vector<uint8_t>& data);
void Recv_MoveResp(void* session, Vector<uint8_t>& data);
void Recv_EnterWorldResp(void* session, Vector<uint8_t>& data);

void MoveAlongPath(Shared<class Scene> world, float dt);
void Send_MoveReq(class NetTcp* net, const Vec3& dst);