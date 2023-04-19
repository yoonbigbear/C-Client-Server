#include "region_system.h"

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