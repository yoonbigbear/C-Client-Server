#include "scene_manager.h"

bool SceneManager::Add(int32_t scene_id, const char* filepath)
{
    if (container_.contains(scene_id))
        return false;

    auto scene = std::make_shared<Scene>();
    scene->Initialize(filepath);
    container_.insert(std::pair<int32_t, Shared<Scene>>(scene_id, scene));

    return true;
}

Weak<Scene> SceneManager::Get(int32_t scene_id)
{
    if (container_.contains(scene_id))
        return container_[scene_id];
    return Weak<Scene>();
}

void SceneManager::Del(int32_t scene_id)
{
    if (container_.contains(scene_id))
        container_.erase(scene_id);
}
