#include "input_system.h"
#include "debug_system.h"

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

        if (ImGui::IsKeyPressed(ImGuiKey_6))
        {

        }

        if (ImGui::IsKeyPressed(ImGuiKey_Space))
        {

        }

        if (ImGui::IsKeyPressed(ImGuiKey_LeftShift))
        {

        }
    }
}
