#include "systems.h"


#include "net_client.h"
#include "manager/scene_manager.h"

#include "fbb/chat_generated.h"
#include "fbb/packets_generated.h"
#include "fbb/world_generated.h"
#include "fbb/common_generated.h"

void Chat_Sync(void* session, Vector<uint8_t>& data)
{
    auto sync = flatbuffers::GetRoot<ChatSync>(data.data());
    LOG_INFO("[info] received ChatSync");

    CHAT_LOG(sync->chat()->c_str());
}

void EnterWorld_Resp(void* session, Vector<uint8_t>& data)
{
    auto resp = flatbuffers::GetRoot<EnterWorldResp>(data.data());
    auto t = resp->UnPack();

    SceneManager::instance().current_scene()->Enter(t->entity.get());
    LOG_INFO("received EnterWorldResp");
}

void Enter_Sync(void* session, Vector<uint8_t>& data)
{
    auto sync = flatbuffers::GetRoot<EnterSync>(data.data());
    auto t = sync->UnPack();
    LOG_INFO("received EnterSync");

    auto scene = SceneManager::instance().current_scene();
    if (scene)
    {
        scene->Enter(t->enter_entity.get());
    }
}

void Leave_Sync(void* session, Vector<uint8_t>& data)
{
    auto sync = flatbuffers::GetRoot<LeaveSync>(data.data());
    auto t = sync->UnPack();
    LOG_INFO("received LeaveSync");

    auto scene = SceneManager::instance().current_scene();
    if (scene)
    {
        scene->Leave(t->leave_entity);
    }
}

void Move_Sync(void* session, Vector<uint8_t>& data)
{
    auto sync = flatbuffers::GetRoot<MoveSync>(data.data());
    auto t = sync->UnPack();
    auto scene = SceneManager::instance().Get(0).lock();
    if (scene)
    {
        Vec dest;
        memcpy(&dest, t->dest.get(), sizeof(Vec));
        scene->MoveRequest(t->eid, dest, t->spd);
    }
    LOG_INFO("received MoveSync");
}

void Move_Resp(void* session, Vector<uint8_t>& data)
{
    auto resp = flatbuffers::GetRoot<MoveResp>(data.data());
    auto t = resp->UnPack();

    if (t->error_code != ErrorCode::None)
    {
        LOG_ERR("recevied MoveError : {}", (int)t->error_code);
    }
    LOG_INFO("received MoveResp");
}

void Move_Req(Weak<NetClient> session, const Vec3& dst, uint32_t eid)
{
    flatbuffers::FlatBufferBuilder fbb(256);
    MoveReqT req;
    req.dest = VecToUnique<fbVec>(dst);
    req.eid = (uint32_t)eid;
    req.eid = eid;
    fbb.Finish(MoveReq::Pack(fbb, &req));
    if (auto ptr = session.lock(); ptr)
        ptr->Send((uint16_t)PacketId::Move_Req, fbb.GetSize(), fbb.GetBufferPointer());
}

void MoveAlongPath(Weak<Scene> scene, float dt)
{

    auto ptr = scene.lock();
    if (ptr)
    {
        {
            auto view = ptr->view<PathList, Transform>();
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
                        ptr->remove<PathList>(entity);
                    }
                    else
                    {
                        //keep moving toward next path.
                        auto& mover = ptr->emplace_or_replace<Mover>(entity);
                        mover.dest = path.paths.front();
                        path.paths.pop_front();

                        //direction
                        mover.dir.v2 = mover.dest.v2 - tf.v.v2;
                        mover.dir.v2.Normalize();

                        //move angle
                        tf.angle = static_cast<short>
                            (std::atan2f(mover.dir.v2.y, mover.dir.v2.x));

                        mover.speed = 0.1;
                        path.flag = Moving;
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

        {
            auto view = ptr->view<Transform, const Mover>();
            for (auto [entity, tf, mover] : view.each())
            {
                auto distance = mover.dest.v2 - tf.v.v2;
                auto len_sqr = distance.LengthSquared();
                auto movelen = dt * mover.speed;

                if (len_sqr < (movelen * movelen))
                {
                    tf.v = mover.dest;
                    //arrive
                    auto path = ptr->try_get<PathList>(entity);
                    if (path)
                    {
                        path->flag = MoveFlag::Arrive;
                    }
                    ptr->remove<Mover>(entity);
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
