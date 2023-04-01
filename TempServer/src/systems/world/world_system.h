#pragma once

#include "pre.h"

void Send_EnterNeighborsResp(class Region& world, entt::entity caller);
void Send_MoveSync(const Vec& dest, float speed, entt::entity sender, Shared<class User> receiver);
void Recv_EnterWorldReq(void* session, Vector<uint8_t>& data);
void Recv_MoveReq(void* session, Vector<uint8_t>& data);

void UpdateMove(class Region& world, float dt);
void MoveAlongPath(class Region& world);
void UpdateNeighbors(class Region& world, Set<entt::entity>& proxies, entt::entity caller);