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
    void Initialize(const char* filepath);
    void Update(float dt);
    void Draw();

public:
    void Enter(const EntityInfo* info);
    void Enter(const Vector<EntityInfo>& entities);
    void Leave(uint32_t entity);
    void Leave(const Vector<uint32_t>& entities);
    bool ScreenRayMove(Vec& start, Vec& end);
    void MoveRequest(uint32_t eid, Vec& end, float spd);

public:
    void Create() noexcept
    {
        if (!entt::locator<Scene>::has_value())
        {
            entt::locator<Scene>::emplace();
        }
    }
    [[nodiscard]] static Scene& Get() noexcept
    {
        return entt::locator<Scene>::value();
    }

    Navigation_C& navmesh() { return nav_mesh_; }
public:
    void AddCommandQueue(std::function<void(void)> command);
    
private:
    void ReleaseCommandQueue();

private:
    UnorderedMap<uint32_t, entt::entity> hash_entities_;

    DebugDrawGLBB dd_;
    Navigation_C nav_mesh_;

    std::vector<std::function<void(void)>> command_queue_;
    SpinLock command_lock_;
};