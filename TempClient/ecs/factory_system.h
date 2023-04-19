#pragma once

#include "pre.h"

entt::entity CreatePc(Shared<class Region> scene, const struct EntityInfo* info);
entt::entity CreateNpc(Shared<class Region> scene, const struct EntityInfo* info);
