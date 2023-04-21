#pragma once
#include "pre.h"



void UpdateMove(class Region& world, float dt);
void MoveAlongPath(class Region& world);

void UpdateDestroyed(entt::registry& world, entt::entity caller);

struct Neighbor
{
    Set<entt::entity> neighbors;
};

class WorldSystem
{
public:
    static void UpdateNeighbors(class Region& world, Set<entt::entity>& new_list, entt::entity caller);
    static void Entered(class Region& world, const entt::entity caller, const Deque<entt::entity>& list);
    static void Leaved(class Region& world, const entt::entity caller, const Deque<entt::entity>& list);



    static struct EntityInfo SerializeEntityInfo(Region& region, entt::entity eid);
};