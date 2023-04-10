#include "factory_system.h"

#include "../level/scene.h"

#include "components.h"


entt::entity CreatePc(Shared<Scene> scene, const EntityInfo* info)
{
    auto entity = scene->Create(info->entity_id());
    
    // Add Transform
    auto& tf = scene->emplace<Transform>(entity);
    memcpy(&tf.v.v3, &info->pos(), sizeof(Vec));
    tf.degree = info->angle();

    // rendering info.
    auto& cylinder = scene->emplace<CylinderData>(entity);
    cylinder.height = 2.f;
    cylinder.radius = 0.6f;

    return entity;
}

entt::entity CreateNpc(Shared<Scene> scene, const struct EntityInfo* info)
{
    auto entity = scene->Create(info->entity_id());
    auto& tf = scene->emplace<Transform>(entity);
    memcpy(&tf.v.v3, &info->pos(), sizeof(Vec));
    tf.degree = info->angle();

    auto& cylinder = scene->emplace<CylinderData>(entity);
    cylinder.height = 2.f;
    cylinder.radius = 0.6f;

    if (info->spd() > 0)
    {
        auto& pathList = scene->emplace_or_replace<PathList>(entity);
        pathList.paths.emplace_back(Vec(info->endpos().x()
            , info->endpos().y(), info->endpos().z()));
    }
    return entity;
}