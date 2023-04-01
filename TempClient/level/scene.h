#pragma once

#include "pre.h"

#include "fbb/common_generated.h"

#include "navigation_c.h"

#include "ecs/components.h"
#include "ecs/systems.h"

#include <entt/entt.hpp>

class Scene : public entt::registry, public std::enable_shared_from_this<Scene>
{
    auto shared() { return shared_from_this(); }
public:
    bool Initialize(const char* filepath);
    void Update(float dt);
    void Draw();

public:
    entt::entity EnterPlayer(const EntityInfo* info);
    entt::entity Enter(const EntityInfo* info);
    void Leave(uint32_t server_eid);
    void Leave(entt::entity client_eid);
    bool ScreenRayMove(Vec& start, Vec& end, entt::entity eid);
    void MoveRequest(entt::entity eid, Vec& end, float spd);


public:
    entt::entity ServerEidToClientEid(uint32_t server_eid)
    {
        return mapped_eid_.contains(server_eid) ? mapped_eid_[server_eid] : entt::null;
    }
    entt::entity Create(uint32_t server_eid)
    {
        auto entity = create();
        mapped_eid_[server_eid] = entity;
        return entity;
    }

public:
    Navigation_C& navmesh() { return nav_mesh_; }

public:
    void AddCommandQueue(std::function<void(void)> command);
private:
    void ReleaseCommandQueue();

private:
    UnorderedMap<uint32_t, entt::entity> mapped_eid_;

    DebugDrawGLBB dd_;
    Navigation_C nav_mesh_;

    std::vector<std::function<void(void)>> command_queue_;
    SpinLock command_lock_;
};