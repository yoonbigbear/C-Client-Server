#pragma once

#include "pre.h"

entt::entity CreatePc(Shared<class Scene> scene, const struct EntityInfo* info);
entt::entity CreateNpc(Shared<class Scene> scene, const struct EntityInfo* info);
entt::entity CreateDebugObject(Shared<class Scene> scene, const Vec& center, Collider&& collider, float ttl = 1.0f);