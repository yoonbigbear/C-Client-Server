#pragma once

#include "pre.h"
#include "share/ecs/share_system.h"

class Scene;
class NetClient;

void Chat_Sync(void* session, Vector<uint8_t>& data);
void Enter_Sync(void* session, Vector<uint8_t>& data);
void Leave_Sync(void* session, Vector<uint8_t>& data);
void Move_Sync(void* session, Vector<uint8_t>& data);
void Move_Resp(void* session, Vector<uint8_t>& data);
void EnterWorld_Resp(void* session, Vector<uint8_t>& data);

void MoveAlongPath(Weak<Scene> world, float dt);
void Move_Req(Weak<NetClient> session, const Vec3& dst, uint32_t eid);