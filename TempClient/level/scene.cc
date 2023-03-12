#include "scene.h"

void Scene::Initialize(const char* filepath)
{
    nav_mesh_.Initialize(filepath);
    //Gui::instance().level_info.onUpdateList = [this](int selected) {
        //size_t size = agents_.size();
        //for (int i = 0; i < size; i++)
        //{
        //    // FIXME: Good candidate to use ImGuiSelectableFlags_SelectOnNav
        //    char label[128];
        //    sprintf_s(label, "MyObject %d", i);
        //    if (ImGui::Selectable(label, selected == i))
        //    {
        //        selected = i;
        //    }
        //}
    //};
}

void Scene::Draw()
{
    nav_mesh_.Render();

    {
        m.lock();
        auto group = registry_.group<>(entt::get<const Transform, const CylinderData>);
        m.unlock();
        for (auto [entity, tf, cylinder] : group.each())
        {
            Draw::Cylinder(&tf.v.v3.x, cylinder.radius, cylinder.height, cylinder.max_climb, startCol);
        }
    }
}

void Scene::Enter(const EntityInfo& info)
{
    auto entity = registry_.create();

    auto& tf = registry_.emplace<Transform>(entity);
    auto& cylinder = registry_.emplace<CylinderData>(entity);
    memcpy(&tf.v.v3, &info.pos(), sizeof(Vec3));

    cylinder.height = 2.f;
    cylinder.radius = 0.6f;
}

void Scene::Move(uint32_t entity, const vec& dest, const vec& dir, float spd)
{
    auto& mover = registry_.emplace_or_replace<MoveInfo>((entt::entity)entity);
    mover.dir = dir;
    mover.dest = dest;
    mover.speed = spd;
}

void Scene::Update(float dt)
{
    //move
    {
        m.lock();
        auto view = registry_.view<Transform, MoveInfo>();
        m.unlock();
        for (auto [entity, tf, mover] : view.each())
        {
            auto dist = mover.dest.v2 - tf.v.v2;
            auto len_sqr = dist.LengthSquared();
            auto move_sqr = dt * mover.speed * mover.dir.v2;

            if (len_sqr < b2Dot(move_sqr,move_sqr))
            {
                tf.v = mover.dest;
            }
            else
            {
                auto mv = dt * mover.speed * dist;
                tf.v.v2 += move_sqr;
            }
        }
    }
}
