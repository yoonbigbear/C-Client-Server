#pragma once

#include "../components.h"

#include "entt/entt.hpp"
#include "packet_handler.h"
#include "spawner.h"


class World
{
public:
    void Start();
    void Enter(std::shared_ptr<ClientSession> session);
    void Enter(int npcid);
    void FixedUpdate();
    void Update(float dt);

    void SpawnAI();

    void Broadcast(std::vector<uint8_t>&& data);
    void BroadcastInSight(std::vector<uint8_t>&& data);

private:
    
    entt::registry registry_;
    PacketHandler packet_handler_;
    //Field field_;
    Spawner spawner_;

};
