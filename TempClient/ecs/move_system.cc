#include "move_system.h"
#include "region_system.h"

#include "net_tcp.h"
#include "level/region.h"
#include "manager/scene_manager.h"

#include "fbb/world_generated.h"
#include "fbb/packets_generated.h"

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

void MoveSystem::Send_MoveReq(NetTcp* net, const Vec3& dst)
{
    ADD_LOG("Send MoveReq");
    flatbuffers::FlatBufferBuilder fbb(256);
    MoveReqT req;
    req.dest = VecToUnique<fbVec>(dst);
    fbb.Finish(MoveReq::Pack(fbb, &req));
    net->Send((uint16_t)PacketId::MoveReq, fbb.GetSize(), fbb.GetBufferPointer());
}

void MoveSystem::MoveAlongPath(Region& region, float dt)
{
    //decide move behaviour path
    {
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

                    tf.speed = 1;
                    path.flag = Moving;

                    //Send MovePacket
                    if (region.all_of<PlayerSession>(entity))
                    {
                        auto& net = region.get<PlayerSession>(entity);
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
        auto view = region.view<Transform, const Mover>();
        for (auto [entity, tf, mover] : view.each())
        {
            auto distance = mover.dest.v2 - tf.v.v2;
            auto len_sqr = distance.LengthSquared();
            auto movelen = dt * tf.speed;

            if (len_sqr < (movelen * movelen))
            {
                tf.v = mover.dest;
                //arrive
                auto path = region.try_get<PathList>(entity);
                if (path)
                {
                    path->flag = MoveFlag::Arrive;
                }
                region.remove<Mover>(entity);
            }
            else
            {
                //moving
                tf.v.v2 += movelen * mover.dir.v2;
            }
        }
    }
}
