#include "scene.h"
#include "ecs/systems.h"
#include "net_tcp.h"
#include "player_client.h"

bool Scene::Initialize(const char* filepath)
{
  return nav_mesh_.Initialize(filepath);
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
    {
        auto entity = PlayerClient::instance().eid;
        if (all_of<Transform, PathList>(entity))
        {
            auto tf = get<Transform>(entity);
            auto path = get<PathList>(entity);
            Draw::Line(&tf.v.v3.x, 2.0, startCol);
            for (auto e : path.paths)
            {
                Draw::Line(&path.paths.front().v3.x, 2.0, startCol);
            }
        }
    }

}

void Scene::Update(float dt)
{
    ReleaseCommandQueue();

    MoveAlongPath(shared(), dt);
}

entt::entity Scene::EnterPlayer(const EntityInfo* info)
{
    DEBUG_RETURN_VALUE(info, entt::null, "null entity info");
    return CreatePc(shared(), info);
}

entt::entity Scene::Enter(const EntityInfo* info)
{
    DEBUG_RETURN_VALUE(info, entt::null, "null entity info");
    return CreateNpc(shared(), info);
}

void Scene::Leave(uint32_t server_eid)
{
    if (mapped_eid_.contains(server_eid))
    {
        Leave(mapped_eid_[server_eid]);
        mapped_eid_.erase(server_eid);
    }
}

void Scene::Leave(entt::entity client_eid)
{
    destroy(client_eid);
}

bool Scene::ScreenRayMove(Vec& start, Vec& end, entt::entity eid)
{
    float t;
    if (nav_mesh_.ScreenRay(&start.v3.x, &end.v3.x, t))
    {
        if (all_of<Transform>(eid))
        {
            auto& tf = get<Transform>(eid);
            if (t < 1.0f)
            {
                float pos[3];
                pos[0] = start.v3.x + (end.v3.x - start.v3.x) * t;
                pos[1] = start.v3.z + (end.v3.z - start.v3.z) * t;
                pos[2] = start.v3.y + (end.v3.y - start.v3.y) * t;
                //ray hit.

                Draw::DrawCircle(pos[0], pos[2], pos[1], 5,
                    duRGBA(0, 0, 0, 255), 2);

                Deque<Vec> path;
                if (nav_mesh_.FindPath(tf.v,
                    Vec(pos[0], pos[1], pos[2]), path, dtQueryFilter()))
                {
                    auto& pathlist = emplace_or_replace<PathList>(eid);
                    std::swap(pathlist.paths, path);
                }
            }
        }
    }
    else
    {
        //failed raycasting
        LOG_ERR("Failed screen ray");
    }
    return true;
}

void Scene::MoveRequest(entt::entity entity, Vec& end, float spd)
{
    if (all_of<Transform>(entity))
    {
        auto& tf = get<Transform>(entity);
        tf.speed = spd;

        Deque<Vec> path;
        auto& pathlist = emplace_or_replace<PathList>(entity);
        pathlist.paths.emplace_back(end);
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
    try
    {
        Vector<std::function<void(void)>> commands;
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
    catch (std::exception& e)
    {
        throw e;
    }
}

