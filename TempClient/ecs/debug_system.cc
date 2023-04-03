#include "debug_system.h"

#include "level/scene.h"
#include "bb_math.h"
#include "factory_system.h"
#include "components.h"
#include "draw.h"

#include "player_client.h"

void DrawDebugToggledObjects(Shared<Scene> scene)
{
    if (ImGui::IsKeyDown(ImGuiKey_LeftAlt))
    {
        //player move path
        auto entity = PlayerClient::instance().eid;
        if (scene->all_of<Transform, PathList, Mover>(entity))
        {
            auto& tf = scene->get<Transform>(entity);
            auto& path = scene->get<PathList>(entity);
            auto& mover = scene->get<Mover>(entity);
            if (path.paths.size() > 0)
            {
                Deque<Vec> list = path.paths;
                list.emplace_front(tf.v);
                for (int i = 0; i < list.size() - 1; ++i)
                {
                    Draw::DebugLine(list[0], list[1], startCol, 2.0f);
                }
            }
            else
            {
                Draw::DebugLine(tf.v, mover.dest, startCol, 2.f);
            }
        }

        if (scene->all_of<Transform>(entity))
        {
            auto& tf = scene->get<Transform>(entity);

            Vec min = Vec{ tf.v.v3.x - 15, tf.v.v3.y -15 ,tf.v.v3.z};
            Vec max = Vec{ tf.v.v3.x + 15, tf.v.v3.y +15 ,tf.v.v3.z};
            Draw::DebugBoxWire(min, max, startCol, 2.0f);
        }
    }
}

void DrawDebugObjects(Shared<Scene> scene)
{
    auto view = scene->view<ColliderComponent>();

    for (auto [entity, collider] : view.each())
    {

        switch (collider.info.type)
        {
        case ColliderType::Circle:
        {
            Draw::DebugCircle(collider.info.center, collider.info.circle.r, startCol, 2.0f);
        }
        break;
        case ColliderType::AABB:
        {
            Vec3 min{ collider.info.center.v3.x - collider.info.aabb.hx,
            collider.info.center.v3.y - collider.info.aabb.hy,
            collider.info.center.v3.z };
            Vec3 max{ collider.info.center.v3.x + collider.info.aabb.hx,
            collider.info.center.v3.y + collider.info.aabb.hy,
            collider.info.center.v3.z };

            Draw::DebugBoxWire(min, max, startCol, 2.0f);
        }
        break;
        case ColliderType::OBB:
        {
        }
        break;
        case ColliderType::Ray:
        {
            auto rad = DegreeToRadian(collider.info.ray.dir);
            auto dir_vec = RadianToUnitVector(rad);
            Draw::DebugLine(collider.info.center, DirLength(dir_vec, collider.info.ray.length)
                , startCol, 2.0f);
        }
        break;
        case ColliderType::Sector:
            break;
        default:
        {LOG_ERR("Wrong type of Collider {}", (int)collider.info.type); }
        break;
        }
    }
}
