#pragma once

#include "pre.h"

#include "fbb/common_generated.h"

#include "navigation_c.h"

#include "ecs/components.h"
#include "ecs/systems.h"

class Region : public Entt::ECS, public std::enable_shared_from_this<Region>
{
    auto shared() { return shared_from_this(); }
public:
    bool Initialize(const char* filepath);
    void Update(float dt);

    void Draw(); //Is not a thread safe. should fix//

public:
    Entity EnterPlayer(const EntityInfo* info);
    Entity Enter(const EntityInfo* info);
    void Leave(uint32_t server_eid);
    void Leave(entt::entity client_eid);

public:
    void Send(Entity eid, uint16_t id, uint8_t* buf, size_t size);

public:
    Navigation_C& navmesh() { return nav_mesh_; }

public:
    void AddCommandQueue(std::function<void(void)> command);
private:
    void ReleaseCommandQueue();

private:
    DebugDrawGLBB dd_;
    Navigation_C nav_mesh_;

    std::vector<std::function<void(void)>> command_queue_;
    SpinLock command_lock_;
};