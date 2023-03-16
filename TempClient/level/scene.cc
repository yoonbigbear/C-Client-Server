#include "scene.h"

void Scene::Initialize(const char* filepath)
{
    nav_mesh_.Initialize(filepath);
  //  auto view = registry_.group<>();
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

    //�Ƹ� ��Ƽ������ ��Ȳ���� registry�� �켱 ������ �����Ǿ�� �ϴ� ����Ʈ�� �ִµ�?
    //�켱 �ӽ÷� Init �κп��� registry�� �������ֵ��� �۾�.
    auto group = registry_.group<>(entt::get<const Transform, const CylinderData>);
}

void Scene::Draw()
{
    nav_mesh_.Render();

    {
        auto group = registry_.group<>(entt::get<const Transform, const CylinderData>);
        for (auto [entity, tf, cylinder] : group.each())
        {
            Draw::Cylinder(&tf.v.v3.x, cylinder.radius, cylinder.height, cylinder.max_climb, startCol);
        }
    }
}

void Scene::Update(float dt)
{
    if (auto queue = ReleaseCommandQueue(); !queue.empty())
    {
        for (auto& invoke : queue)
        {
            invoke();
        }
    }

    //move
    {
        auto view = registry_.view<Transform, MoveInfo>();
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

void Scene::AddCommandQueue(std::function<void(void)> command)
{
    {
        LockGuard<SpinLock> guard(command_lock_);
        command_queue_.emplace_back(command);
    }
}

std::vector<std::function<void(void)>> Scene::ReleaseCommandQueue()
{
    std::vector<std::function<void(void)>> commands;
    {
        LockGuard<SpinLock> guard(command_lock_);
        if (!command_queue_.empty())
        {
            commands.assign(command_queue_.begin(), command_queue_.end());
            command_queue_.clear();
        }
    }
    return commands;
}

