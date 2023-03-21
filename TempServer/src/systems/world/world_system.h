#pragma once

#include "pre.h"

void SendEnterResp(Weak<class World> world, const Vector<struct Proxy*>& new_list, entt::entity caller);
void SightSyncronize(Weak<class World> world, Vector<struct Proxy*>& proxies, entt::entity caller);
void SendEnterSync(Weak<class World> world, Unique<struct EntityInfo> sender,
    const Vector<struct Proxy*>& new_list, const Vector<struct Proxy*>& before_list);

void SendLeaveSync(Weak<class World> world, uint32_t sender, const Vector<struct Proxy*>& new_list,
    const Vector<struct Proxy*>& before_list);
void SendMoveSync(const struct Mover& mover, Weak<class ClientSession> session, uint32_t eid);
void RecvMoveReq(void* session_ptr, std::vector<uint8_t>& data);

void UpdateMove(Weak<class World> world, float dt);
void MoveAlongPath(Weak<class World> world);