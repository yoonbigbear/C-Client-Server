#include "region_system.h"
#include "move_system.h"

#include "net_tcp.h"
#include "player_client.h"

#include "manager/scene_manager.h"

#include "fbb/world_generated.h"

void Recv_UpdateNeighborsNfy(void* session, Vector<uint8_t>& data)
{
    ADD_LOG("Received UpdateNeighborNfy");
    auto resp = flatbuffers::GetRoot<UpdateNeighborsNfy>(data.data());
    auto t = resp->UnPack();

    auto net = reinterpret_cast<NetTcp*>(session);
    DEBUG_RETURN(net, "null session");

    auto scene = SceneManager::instance().Get(PlayerClient::instance().cur_scene_id_);
    for (auto& e : t->enter_entity)
    {
        scene->Enter(&e);
    }
    for (auto e : t->leave_entity)
    {
        scene->Leave(e);
    }
}

void Recv_EnterWorldAck(void* session, Vector<uint8_t>& data)
{
    ADD_LOG("Received EnterWorldAck");
    auto resp = flatbuffers::GetRoot<EnterWorldAck>(data.data());
    auto t = resp->UnPack();

    auto net = reinterpret_cast<NetTcp*>(session);
    if (net)
    {
        //my player
        auto eid = SceneManager::instance().current_scene()->EnterPlayer(t->entity.get());
        auto& session_comp = SceneManager::instance().current_scene()->emplace<PlayerSession>(eid);
        session_comp.session = net;
        PlayerClient::instance().cur_scene_id_ = 0;
        PlayerClient::instance().eid = eid;
    }
    else
    {
        ADD_ERROR("null session");
    }
}

void Recv_EnterNeighborsNfy(void* session, Vector<uint8_t>& data)
{
    auto net = reinterpret_cast<NetTcp*>(session);
    DEBUG_RETURN(net, "null session");

    ADD_LOG("Received EnterNeighborsSync");

    auto sync = flatbuffers::GetRoot<EnterNeighborsNfy>(data.data());
    auto t = sync->UnPack();

    auto scene = SceneManager::instance().current_scene();
    if (scene)
    {
        scene->Enter(t->enter_entity.get());
    }
}

void Recv_LeaveNeighborsNfy(void* session, Vector<uint8_t>& data)
{
    auto net = reinterpret_cast<NetTcp*>(session);
    DEBUG_RETURN(net, "null session");

    auto sync = flatbuffers::GetRoot<LeaveNeighborsNfy>(data.data());
    auto t = sync->UnPack();
    ADD_LOG("Received LeaveNeighborsNfy {}", t->leave_entity);

    auto scene = SceneManager::instance().current_scene();
    if (scene)
    {
        scene->Leave(t->leave_entity);
    }
}

bool RegionSystem::PathTo(Region& region, Entity eid, Vec& start, Vec& end)
{
    if (region.Valid<Transform>(eid))
    {
        float t;
        if (region.navmesh().ScreenRay(start.get(), end.get(), t))
        {
            if (t < 1.0f)
            {
                float pos[3];
                pos[0] = start.v3.x + (end.v3.x - start.v3.x) * t;
                pos[1] = start.v3.z + (end.v3.z - start.v3.z) * t;
                pos[2] = start.v3.y + (end.v3.y - start.v3.y) * t;
                //ray hit.

                auto& tf = region.get<Transform>(eid);
                Deque<Vec> path;
                if (region.navmesh().FindPath(tf.v,
                    Vec(pos[0], pos[1], pos[2]), path, dtQueryFilter()))
                {
                    auto& pathlist = region.Emplace_or_Replace<PathList>(eid);
                    std::swap(pathlist.paths, path);
                    pathlist.flag = MoveFlag::Start;
                }
            }
        }
        else
        {
            //failed raycasting
            ADD_ERROR("Failed screen ray");
            return false;
        }
        return true;
    }
    return false;
}

void RegionSystem::Update(Region& region)
{
    //decide move behaviour path
    auto view = region.view<PathList, Transform>();
    for (const auto [entity, path, tf] : view.each())
    {
        switch (path.flag)
        {
        case MoveFlag::Arrive:
        case MoveFlag::Start:
        {
            if (path.paths.empty())
            {
                //reached destination.
                region.remove<PathList>(entity);
                tf.speed = 0;
            }
            else
            {
                //keep moving toward next path.
                auto& mover = region.emplace_or_replace<Mover>(entity);
                mover.dest = path.paths.front();
                path.paths.pop_front();

                //direction
                mover.dir.v2 = mover.dest.v2 - tf.v.v2;
                mover.dir.v2.Normalize();

                //move angle
                tf.degree = BBMath::UnitVectorToDegree(mover.dir.v2.x, mover.dir.v2.y);

                tf.speed = tf.base_spd;
                path.flag = Moving;

                //Send MovePacket
                if (region.all_of<PlayerSession>(entity))
                {
                    auto& net = region.get<PlayerSession>(entity);
                    MoveSystem::Send_MoveReq(net.session, mover.dest.v3);
                }
            }
        }
        break;
        case MoveFlag::Moving:
        {
            //moving.
            break;;
        }
        break;

        default:
            break;
        }
    }
}
