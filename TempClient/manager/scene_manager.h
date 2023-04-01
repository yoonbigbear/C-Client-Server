#pragma once

#include "level/scene.h"

class Scene;
class SceneManager : public Singleton<SceneManager>
{
public:
    bool Add(int32_t scene_id, const char* filepath);
    void Del(int32_t scene_id);
    Shared<Scene> Get(int32_t scene_id);

public:
    auto container() { return container_; }
    auto current_scene() { return container_[current_scene_]; }

private:
    UnorderedMap<int32_t, Shared<Scene>> container_;
    int current_scene_ = 0;
};