#pragma once

#include "level/scene.h"

class Scene;
class SceneManager : public Singleton<SceneManager>
{
public:
    bool Add(int32_t scene_id, const char* filepath);
    void Del(int32_t scene_id);
    Weak<Scene> Get(int32_t scene_id);

public:
    auto container() { return container_; }

private:
    UnorderedMap<int32_t, Shared<Scene>> container_;
};