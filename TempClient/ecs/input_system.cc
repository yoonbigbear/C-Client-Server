#include "input_system.h"
#include "debug_system.h"
#include "player_client.h"

#include "bb_math.h"
#include "gui.h"
#include "level/region.h"

void InputSystem::KeyboardInput(Region& region, Entity entity)
{
    if (region.Valid<Transform>(entity))
    {
        auto& tf = region.get<Transform>(entity);
        Collider collider;
        collider.center = tf.v;

        if (ImGui::IsKeyPressed(ImGuiKey_1))
        {
            collider.circle.r = 10.f;
            DebugSystem::SendDebugCollision(region, Circle, tf.degree, entity);
            DebugSystem::CreateDebugColliderObject(region, std::move(collider), BB::RGBA<0, 0, 255, 100>::VAL);
        }

        if (ImGui::IsKeyPressed(ImGuiKey_2))
        {
            collider.type = Sector;
            collider.sector.dir = tf.degree;
            collider.sector.half_angle = 30;
            collider.sector.r = 10.f;
            DebugSystem::SendDebugCollision(region, Sector, tf.degree, entity);
            DebugSystem::CreateDebugColliderObject(region, std::move(collider), BB::RGBA<0, 0, 255, 100>::VAL);
        }

        if (ImGui::IsKeyPressed(ImGuiKey_3))
        {
            collider.type = AABB;
            collider.aabb.hx = 10.f;
            collider.aabb.hy = 10.f;
            DebugSystem::SendDebugCollision(region, AABB, tf.degree, entity);
            DebugSystem::CreateDebugColliderObject(region, std::move(collider), BB::RGBA<0, 0, 255, 100>::VAL);
        }

        if (ImGui::IsKeyPressed(ImGuiKey_4))
        {
            collider.type = OBB;
            collider.obb.dir = tf.degree;
            collider.obb.hw = 2.f;
            collider.obb.hl = 10.f;
            collider.center.v2 += BBMath::DirLength(tf.degree, collider.obb.hl).v2;
            DebugSystem::SendDebugCollision(region, OBB, tf.degree, entity);
            DebugSystem::CreateDebugColliderObject(region, std::move(collider), BB::RGBA<0,0,255, 100>::VAL);
        }

        if (ImGui::IsKeyPressed(ImGuiKey_5))
        {
            collider.type = Ray;
            collider.ray.dir = tf.degree;
            collider.ray.length = 20.f;
            DebugSystem::SendDebugCollision(region, Ray, tf.degree, entity);
            DebugSystem::CreateDebugColliderObject(region, std::move(collider), BB::RGBA<0, 0, 255, 100>::VAL);
        }

        if (ImGui::IsKeyPressed(ImGuiKey_Space))
        {
            auto net = region.get<PlayerSession>(entity).session;
            Vec start = Vec(Camera::instance().ray_start);
            Vec end = Vec(Camera::instance().ray_end);

            float t;
            region.navmesh().ScreenRay(&start.v3.x, &end.v3.x, t);
            if (t < 1.0f)
            {
                float pos[3];
                pos[0] = start.v3.x + (end.v3.x - start.v3.x) * t;
                pos[1] = start.v3.z + (end.v3.z - start.v3.z) * t;
                pos[2] = start.v3.y + (end.v3.y - start.v3.y) * t;
                //ray hit.
                auto dest = Vec(pos[0], pos[1], pos[2]);
                auto dir = BBMath::UnitVectorToDegree(pos[0] - tf.v.v2.x, pos[1] - tf.v.v2.y);
                MoveSystem::Send_DashReq(net, dir);

            }
        }

        if (ImGui::IsKeyPressed(ImGuiKey_LeftShift))
        {

        }
    }
}
