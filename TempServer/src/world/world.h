#pragma once

#include "pre.h"

#include "entt/entt.hpp"

#include "net/client_session.h"
#include "packet_handler.h"
#include "field.h"


class World
{
public:
    void Initialize();
    void Enter(Shared<ClientSession> session);
    void Enter(int npcid);
    void Update(float dt);

    void SpawnAI();

    void Broadcast(Vector<uint8_t>&& data);

private:
    entt::registry registry_;
    Field field_;
};
