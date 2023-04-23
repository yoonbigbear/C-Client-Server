#include "region.h"
#include "ecs/systems.h"
#include "ecs/components.h"

#include "net_tcp.h"
#include "player_client.h"
#include "manager/render_manager.h"

#include "fbb/packets_generated.h"

bool Region::Initialize(const char* filepath)
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

void Region::Draw()
{
    nav_mesh_.Render(&dd_);
    
    InputSystem::KeyboardInput(*this, PlayerClient::instance().eid);

    DrawDebugToggledObjects(shared());

    //draw objects
    {
        ImGuiIO& io = ImGui::GetIO();
        auto vw = view<const Transform, const CylinderData>();
        for (auto [entity, tf, cylinder] : vw.each())
        {
            Draw::DebugCylinder(tf.v, cylinder.radius, cylinder.height, cylinder.max_climb, startCol);

            if (ImGui::IsKeyDown(ImGuiKey_LeftAlt))
            {
                GLdouble x, y, z;
                if (gluProject((GLdouble)tf.v[0], (GLdouble)tf.v[2], (GLdouble)tf.v[1],
                    Camera::instance().modelview_matrix,
                    Camera::instance().projection_matrix,
                    Camera::instance().viewport, &x, &y, &z))
                {
                    y = io.DisplaySize.y - y;
                    Draw::DebugText((float)x, (float)y, std::format("x:{}, y:{}, z:{}\n eid:{}",
                        (int)tf.v.v3.x, (int)tf.v.v3.y, (int)tf.v.v3.z, (uint32_t)entity).c_str());
                }
            }
        }
    }

    DebugSystem::Update(*this, ImGui::GetIO().DeltaTime);
}

void Region::Update(float dt)
{
    ReleaseCommandQueue();

    UpdateTimer(shared(), dt);

    RegionSystem::Update(*this);
    MoveSystem::Update(*this, dt);
}

entt::entity Region::EnterPlayer(const EntityInfo* info)
{
    DEBUG_RETURN_VALUE(info, entt::null, "null entity info");
    return CreatePc(shared(), info);
}

entt::entity Region::Enter(const EntityInfo* info)
{
    DEBUG_RETURN_VALUE(info, entt::null, "null entity info");
    return CreateNpc(shared(), info);
}

void Region::Leave(uint32_t server_eid)
{
    if (mapped_eid_.contains(server_eid))
    {
        Leave(mapped_eid_[server_eid]);
        mapped_eid_.erase(server_eid);
    }
}

void Region::Leave(entt::entity client_eid)
{
    if (valid(client_eid))
        destroy(client_eid);
}

void Region::Send(Entity eid, uint16_t id, uint8_t* buf, size_t size)
{
    auto& net = get<PlayerSession>(eid);
    net.session->Send(static_cast<uint16_t>(id), size, buf);
}

void Region::AddCommandQueue(std::function<void(void)> command)
{
    {
        LockGuard<SpinLock> guard(command_lock_);
        command_queue_.emplace_back(command);
    }
}

void Region::ReleaseCommandQueue()
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

