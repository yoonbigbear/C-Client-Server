#include "scene_manager.h"

bool SceneManager::Add(int32_t scene_id, const char* filepath)
{
    if (container_.contains(scene_id))
        return false;

    auto scene = std::make_shared<Region>();
    if (!scene->Initialize(filepath))
        return false;
    container_.insert(std::pair<int32_t, Shared<Region>>(scene_id, scene));

    return true;
}

Shared<Region> SceneManager::Get(int32_t scene_id)
{
    if (container_.contains(scene_id))
        return container_[scene_id];
    return Shared<Region>();
}

void SceneManager::Del(int32_t scene_id)
{
    if (container_.contains(scene_id))
        container_.erase(scene_id);
}
