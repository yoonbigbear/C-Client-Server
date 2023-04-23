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
        MoveSystem::MoveTo(*scene, entity, dest, t->spd);
    }
}

void Recv_MoveAck(void* session, Vector<uint8_t>& data)
{
    auto net = reinterpret_cast<NetTcp*>(session);
    DEBUG_RETURN(net, "null session");

    ADD_LOG("Received MoveAck");
    auto ack = flatbuffers::GetRoot<MoveAck>(data.data());
    auto t = ack->UnPack();

    if (t->error_code != ErrorCode::None)
    {
        ADD_ERROR("Recevied MoveError : {}", (int)t->error_code);
    }
}

void Recv_DashAck(void* session, Vector<uint8_t>& data)
{
    auto net = reinterpret_cast<NetTcp*>(session);
    DEBUG_RETURN(net, "null session");

    ADD_LOG("Received DashAck");

    auto ack = flatbuffers::GetRoot<DashAck>(data.data());
    auto t = ack->UnPack();

    if (t->error_code != ErrorCode::None)
    {
        ADD_ERROR("Recevied DashError : {}", (int)t->error_code);
    }
}

void Recv_DashNfy(void* session, Vector<uint8_t>& data)
{
    auto net = reinterpret_cast<NetTcp*>(session);
    DEBUG_RETURN(net, "null session");

    ADD_LOG("Received DashNfy");
    auto nfy = flatbuffers::GetRoot<DashNfy>(data.data());
    auto t = nfy->UnPack();

    auto vec = VecTo<fbVec, Vec>(*t->dest.get());
    MoveSystem::MoveTo(*SceneManager::instance().current_scene(),
        Entity(t->eid), vec, t->spd);
}

void MoveSystem::Send_MoveReq(NetTcp* net, const Vec3& dst)
{
    ADD_LOG("Send MoveReq");
    flatbuffers::FlatBufferBuilder fbb(32);
    MoveReqT req;
    req.dest = VecToUnique<fbVec>(dst);
    fbb.Finish(MoveReq::Pack(fbb, &req));
    net->Send((uint16_t)PacketId::MoveReq, fbb.GetSize(), fbb.GetBufferPointer());
}

void MoveSystem::Send_DashReq(NetTcp* net, const short dir)
{
    ADD_LOG("Send DashReq");
    flatbuffers::FlatBufferBuilder fbb(32);
    DashReqT req;
    req.dir = dir;
    fbb.Finish(DashReq::Pack(fbb, &req));
    net->Send((uint16_t)PacketId::DashReq, fbb.GetSize(), fbb.GetBufferPointer());
}

void MoveSystem::Update(Region& region, float dt)
{
    //move
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

void MoveSystem::MoveTo(Region& region, Entity eid, Vec& end, float spd)
{
    if (region.Valid<Transform>(eid))
    {
        auto& tf = region.get<Transform>(eid);
        dtRaycastHit hit;
        if (region.navmesh().Raycast(tf.v, end, hit))
        {
            auto& mover = region.Emplace_or_Replace<Mover>(eid);

            if (hit.t < 1.0f)
            {
                //ray hit.

                float pos[3];
                pos[0] = tf.v.v3.x + (end.v3.x - tf.v.v3.x) * hit.t;
                pos[1] = tf.v.v3.z + (end.v3.z - tf.v.v3.z) * hit.t;
                pos[2] = tf.v.v3.y + (end.v3.y - tf.v.v3.y) * hit.t;

                end = Vec(pos[0], pos[1], pos[2]);
            }

            mover.dest = end;
            mover.dir.v2 = mover.dest.v2 - tf.v.v2;
            mover.dir.v2.Normalize();
            tf.degree = BBMath::UnitVectorToDegree(mover.dir.v2.x, mover.dir.v2.y);
            tf.speed = spd;
        }
    }
}