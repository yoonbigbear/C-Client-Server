#include "scene.h"

#include "ecs/systems.h"

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

    if (!entt::locator<Scene>::has_value())
    {
        entt::locator<Scene>::emplace();
    }
}

void Scene::Draw()
{
    nav_mesh_.Render(&dd_);

    //draw objects
    {
        auto vw = view<const Transform, const CylinderData>();
        for (auto [entity, tf, cylinder] : vw.each())
        {
            float dtpos[3] = { tf.v.v3.x, tf.v.v3.z, tf.v.v3.y };
            Draw::Cylinder(dtpos, cylinder.radius, cylinder.height, cylinder.max_climb, startCol);
        }
    }

    //draw path lines
    /*{
        auto vw = view<const Transform, const PathList>();
        for (auto [entity, tf, path] : vw.each())
        {

            Draw::Line(&tf.v.v3.x, 2.0, startCol);
            for (auto e : path.paths)
            {
                Draw::Line(&path.paths.front().v3.x, 2.0, startCol);
            }
        }
    }*/

}

void Scene::Update(float dt)
{
    ReleaseCommandQueue();

    MoveAlongPath(shared(), dt);
}

void Scene::Enter(const EntityInfo* info)
{
    ENTITY(info->entity_id());
    auto eid = create(entity);
    if (valid(eid))
    {
        auto& tf = emplace<Transform>(eid);
        memcpy(&tf.v.v3, &info->pos(), sizeof(Vec));
        tf.angle = info->angle();

        auto& cylinder = emplace<CylinderData>(eid);
        cylinder.height = 2.f;
        cylinder.radius = 0.6f;

        if (info->flag() == EntityFlag::Player)
            auto& player = emplace<MyPlayer>(eid);

        if (info->spd() > 0)
        {
            auto& pathList = emplace_or_replace<PathList>(eid);
            pathList.paths.emplace_back(Vec(info->endpos().x()
                , info->endpos().y(), info->endpos().z()));
        }
    }
}

void Scene::Enter(const Vector<EntityInfo>& info)
{
    for (auto& e : info)
    {
        auto entity = create((entt::entity)e.entity_id());

        if (valid(entity))
        {
            auto& tf = emplace<Transform>(entity);
            memcpy(&tf.v.v3, &e.pos(), sizeof(Vec));
            tf.angle = e.angle();

            auto& cylinder = emplace<CylinderData>(entity);
            cylinder.height = 2.f;
            cylinder.radius = 0.6f;

            if (e.spd() > 0)
            {
                auto& pathList = emplace_or_replace<PathList>(entity);
                pathList.paths.emplace_back(Vec(e.endpos().x()
                    , e.endpos().y(), e.endpos().z()));
            }
        }
    }
}

void Scene::Leave(uint32_t eid)
{
    ENTITY(eid);
    if (valid(entity))
    {
        destroy(entity);
    }
}

void Scene::Leave(const Vector<uint32_t>& info)
{
    for (auto& e : info)
    {
        ENTITY(e);
        if (valid(entity))
        {
            destroy(entity);
        }
    }
}

bool Scene::MoveRequest(Vec& start, Vec& end)
{
    auto player = view<MyPlayer>();
    for (auto [entity, myplayer] : player.each())
    {
        float t;
        if (nav_mesh_.ScreenRay(&start.v3.x, &end.v3.x, t))
        {
            auto tf = try_get<Transform>(entity);
            if (tf)
            {
                if (t < 1.0f)
                {
                    //ray hit.
                    auto hit_point = t * start.v3;


                    List<Vec> path;
                    if (nav_mesh_.FindPath(tf->v, hit_point, path, dtQueryFilter()))
                    {
                        //replace path list if exists
                        auto& pathlist = emplace_or_replace<PathList>(entity);
                        std::swap(pathlist.paths, path);
                    }
                }
                else
                {
                    //no hit. no path.
                    LOG_ERR("No hit screen ray");
                }
            }
        }
        else
        {
            //failed raycasting
            LOG_ERR("Failed screen ray");
            LOG_INFO("startpoint %f %f %f",
                start.v3.x, start.v3.y, start.v3.z);
            LOG_INFO("endpoint %f %f %f",
                end.v3.x, end.v3.y, end.v3.z);
        }
        return true;
    }
    LOG_ERR("Not found my player");
    return false;
}

void Scene::MoveRequest(uint32_t eid, Vec& end, float spd)
{
    ENTITY(eid);

    if (valid(entity))
    {
        auto& mover = emplace_or_replace<Mover>(entity);
        mover.dest = end;

        auto tf = try_get<Transform>(entity);
        if (tf)
        {
            mover.dir = mover.dest.v2 - tf->v.v2;
            mover.dir.v2.Normalize();
            mover.speed = spd;
        }

        if (all_of<MyPlayer>(entity))
        {
            auto session = get<MyPlayer>(entity);
            Move_Req(session.session, mover.dest.v3, eid);
        }
    }
}

void Scene::AddCommandQueue(std::function<void(void)> command)
{
    {
        LockGuard<SpinLock> guard(command_lock_);
        command_queue_.emplace_back(command);
    }
}

void Scene::ReleaseCommandQueue()
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

    for (auto& invoke : commands)
    {
        invoke();
    }
}

