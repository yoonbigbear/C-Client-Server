#pragma once

#include "pre.h"

#include "fbb/common_generated.h"

#include "navi_mesh.h"

#include "ecs/components.h"
#include "ecs/systems.h"

#include <entt/entt.hpp>

class Scene
{
public:
    void Initialize(const char* filepath);
    void Update(float dt);
    void Draw();

public:
    void Enter(const EntityInfo& info);
    void Move(uint32_t entity, const vec& dest, const vec& dir, float spd);

private:
    entt::registry registry_;
    UnorderedMap<uint32_t, entt::entity> hash_entities_;
    NaviMesh nav_mesh_;
};