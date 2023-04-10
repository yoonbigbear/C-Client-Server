#pragma once

#include "pre.h"
void Recv_EnterWorldReq(void* session, Vector<uint8_t>& data);
void Recv_MoveReq(void* session, Vector<uint8_t>& data);


void UpdateMove(class Region& world, float dt);
void MoveAlongPath(class Region& world);

void UpdateDestroyed(entt::registry& world, entt::entity caller);


class WorldSystem
{
public:
    static void UpdateNeighbors(class Region& world, Set<entt::entity>& new_list, entt::entity caller);
};