#include "systems.h"

#include "player_client.h"
#include "level/scene.h"
#include "net_tcp.h"
#include "manager/scene_manager.h"
#include "fbb/chat_generated.h"
#include "fbb/packets_generated.h"
#include "fbb/world_generated.h"
#include "fbb/common_generated.h"
#include "fbb/debug_generated.h"
#include "debug_system.h"

void Recv_ChatNfy(void* session, Vector<uint8_t>& data)
{
    ADD_LOG("Received ChatNfy");

    auto net = reinterpret_cast<NetTcp*>(session);
    DEBUG_RETURN(net, "null session");

    auto sync = flatbuffers::GetRoot<ChatNfy>(data.data());
    __unused auto pack = sync->UnPack();
    CHAT_LOG(pack->chat.c_str());
}

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

void Recv_MoveNfy(void* session, Vector<uint8_t>& data)
{
    auto net = reinterpret_cast<NetTcp*>(session);
    DEBUG_RETURN(net, "null session");

    auto sync = flatbuffers::GetRoot<MoveNfy>(data.data());
    auto t = sync->UnPack();
    ADD_LOG("Received MoveNfy eid {}", t->eid);

    auto scene = SceneManager::instance().current_scene();
    if (scene)
    {
        Vec dest;
        memcpy(&dest, t->dest.get(), sizeof(Vec));
        auto entity = scene->ServerEidToClientEid(t->eid);
        DEBUG_RETURN(scene->valid(entity), "Invalid entity id svr {} / client {}", t->eid, (uint32_t)entity);
        scene->MoveRequest(entity, dest, t->spd);
    }
}

void Recv_MoveAck(void* session, Vector<uint8_t>& data)
{
    auto net = reinterpret_cast<NetTcp*>(session);
    DEBUG_RETURN(net, "null session");

    ADD_LOG("Received MoveAck");
    auto resp = flatbuffers::GetRoot<MoveAck>(data.data());
    auto t = resp->UnPack();

    if (t->error_code != ErrorCode::None)
    {
        ADD_ERROR("Recevied MoveError : {}", (int)t->error_code);
    }
}

void Send_MoveReq(NetTcp* net, const Vec3& dst)
{
    ADD_LOG("Send MoveReq");
    flatbuffers::FlatBufferBuilder fbb(256);
    MoveReqT req;
    req.dest = VecToUnique<fbVec>(dst);
    fbb.Finish(MoveReq::Pack(fbb, &req));
    net->Send((uint16_t)PacketId::MoveReq, fbb.GetSize(), fbb.GetBufferPointer());
}

void Recv_Debug3DPosition(void* session, Vector<uint8_t>& data)
{
    ADD_LOG("Received Debug3DPosition");
    auto resp = flatbuffers::GetRoot<Debug3DPosition>(data.data());
    auto t = resp->UnPack();

    DrawDebugPosition(SceneManager::instance().current_scene(),
        Vec(t->pos->x(), t->pos->y(), t->pos->z()));
}

void MoveAlongPath(Shared<Scene> scene, float dt)
{
    if (scene)
    {
        //decide move behaviour path
        {
            auto view = scene->view<PathList, Transform>();
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
                        scene->remove<PathList>(entity);
                        tf.speed = 0;
                    }
                    else
                    {
                        //keep moving toward next path.
                        auto& mover = scene->emplace_or_replace<Mover>(entity);
                        mover.dest = path.paths.front();
                        path.paths.pop_front();

                        //direction
                        mover.dir.v2 = mover.dest.v2 - tf.v.v2;
                        mover.dir.v2.Normalize();

                        //move angle
                        tf.degree = static_cast<short>
                            (std::atan2f(mover.dir.v2.y, mover.dir.v2.x));

                        tf.speed = 1;
                        path.flag = Moving;

                        //Send MovePacket
                        if (scene->all_of<PlayerSession>(entity))
                        {
                            auto& net = scene->get<PlayerSession>(entity);
                            Send_MoveReq(net.session, mover.dest.v3);
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

        //move
        {
            auto view = scene->view<Transform, const Mover>();
            for (auto [entity, tf, mover] : view.each())
            {
                auto distance = mover.dest.v2 - tf.v.v2;
                auto len_sqr = distance.LengthSquared();
                auto movelen = dt * tf.speed;

                if (len_sqr < (movelen * movelen))
                {
                    tf.v = mover.dest;
                    //arrive
                    auto path = scene->try_get<PathList>(entity);
                    if (path)
                    {
                        path->flag = MoveFlag::Arrive;
                    }
                    scene->remove<Mover>(entity);
                }
                else
                {
                    //moving
                    tf.v.v2 += movelen * mover.dir.v2;
                }
            }
        }
    }
}
