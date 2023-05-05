#include "debug_system.h"
#include "region_system.h"

#include "manager/render_manager.h"
#include "manager/scene_manager.h"

#include "level/region.h"
#include "bb_math.h"
#include "components.h"
#include "draw.h"
#include "player_client.h"

#include "fbb/debug_generated.h"
#include "fbb/packets_generated.h"

void Recv_Debug3DPosition(void* session, Vector<uint8_t>& data)
{
    ADD_LOG("Received Debug3DPosition");
    auto nfy = flatbuffers::GetRoot<Debug3DPosition>(data.data());
    auto t = nfy->UnPack();

    DrawDebugPosition(SceneManager::instance().current_scene(),
        Vec(t->pos->x(), t->pos->y(), t->pos->z()));
}

void Recv_DebugColliderNfy(void* session, Vector<uint8_t>& data)
{
    ADD_LOG("Received DebugColliderNfy");
    auto nfy = flatbuffers::GetRoot<DebugColliderNfy>(data.data());
    auto t = nfy->UnPack();

    Collider col;
    col.type = ColliderType(t->collider->type);
    col.center = Vec(t->collider->center->x(), t->collider->center->y(), t->collider->center->z());
    switch (col.type)
    {
    case ColliderType::Circle:
        memmove(&col.circle, t->collider->data.data(), sizeof(CircleInfo));
        break;
    case ColliderType::Sector:
        memmove(&col.sector, t->collider->data.data(), sizeof(SectorInfo));
        break;
    case ColliderType::AABB:
        memmove(&col.aabb, t->collider->data.data(), sizeof(AABBInfo));
        break;
    case ColliderType::OBB:
        memmove(&col.obb, t->collider->data.data(), sizeof(OBBInfo));
        break;
    case ColliderType::Ray:
        memmove(&col.ray, t->collider->data.data(), sizeof(RayInfo));
        break;
    }

    DebugSystem::CreateDebugColliderObject(*(SceneManager::instance().current_scene().get()),
        std::move(col), bb::RGBA<0, 255, 0, 255>::VAL, 0.5f);
}

void DrawDebugToggledObjects(Shared<Region> scene)
{
    if (ImGui::IsKeyDown(ImGuiKey_LeftAlt))
    {
        //player move path
        auto entity = PlayerClient::instance().eid;
        if (scene->all_of<Transform, PathList, Mover>(entity))
        {
            auto& tf = scene->get<Transform>(entity);
            auto& path = scene->get<PathList>(entity);
            auto& mover = scene->get<Mover>(entity);
            if (path.paths.size() > 0)
            {
                Deque<Vec> list = path.paths;
                list.emplace_front(tf.v);
                for (int i = 0; i < list.size() - 1; ++i)
                {
                    Draw::DebugLine(list[0], list[1], startCol, 2.0f);
                }
            }
            else
            {
                Draw::DebugLine(tf.v, mover.dest, startCol, 2.f);
            }
        }

        if (scene->all_of<Transform>(entity))
        {
            auto& tf = scene->get<Transform>(entity);

            Vec min = Vec{ tf.v.v3.x - 15, tf.v.v3.y - 15 ,tf.v.v3.z };
            Vec max = Vec{ tf.v.v3.x + 15, tf.v.v3.y + 15 ,tf.v.v3.z };
            Draw::DebugBoxWire(min, max, startCol, 2.0f);
        }
    }
}

void DrawDebugPosition(Shared<class Region> scene, Vec pos)
{
    auto entity = scene->create();
    Draw::DebugCircle(pos.v3, 3, startCol, 2.0f);
}

void DebugSystem::SendDebugCollision(Region& region, ColliderType type, short dir, entt::entity eid)
{
    flatbuffers::FlatBufferBuilder fbb(32);
    DebugColliderReqT req;
    req.dir = dir;
    req.type = type;
    fbb.Finish(DebugColliderReq::Pack(fbb, &req));
    region.Send(eid, (uint16_t)PacketId::DebugColliderReq, fbb.GetBufferPointer(), fbb.GetSize());
}

void DebugSystem::CreateDebugColliderObject(Region& region, Collider&& c, uint32_t col, float ttl)
{
    auto entity = region.create();
    auto& collider = region.emplace<DebugCollider>(entity);
    collider.collider = std::move(c);
    collider.color = col;
    collider.ttl = ttl;
}

void DebugSystem::Update(Region& region, float dt)
{
    UpdateDebugCollider(region, dt);
}

void DebugSystem::UpdateDebugCollider(Region& region, float dt)
{
    auto view = region.view<DebugCollider>();
    for (auto [entity, collider] : view.each())
    {
        collider.ttl -= dt;
        if (collider.ttl < 0.f)
        {
            region.erase<DebugCollider>(entity);
        }

        switch (collider.collider.type)
        {
        case ColliderType::Circle:
        {
            Draw::DebugCircle(collider.collider.center, collider.collider.circle.r, collider.color, 2.0f);
        }
        break;
        case ColliderType::AABB:
        {
            Vec3 min{ collider.collider.center.v3.x - collider.collider.aabb.hx,
            collider.collider.center.v3.y - collider.collider.aabb.hy,
            collider.collider.center.v3.z };
            Vec3 max{ collider.collider.center.v3.x + collider.collider.aabb.hx,
            collider.collider.center.v3.y + collider.collider.aabb.hy,
            collider.collider.center.v3.z };

            Draw::DebugBoxWire(min, max, collider.color, 2.0f);
        }
        break;
        case ColliderType::OBB:
        {
            b2PolygonShape obb;
            obb.SetAsBox(
                collider.collider.obb.hl, collider.collider.obb.hw,
                collider.collider.center.v2,
                BBMath::ToRad(collider.collider.obb.dir));
            Draw::DebugObbWire({ obb.m_vertices[0], obb.m_vertices[1],obb.m_vertices[2],obb.m_vertices[3] },
                collider.color, 2.0f);
        }
        break;
        case ColliderType::Ray:
        {
            auto end = BBMath::DirLength(collider.collider.ray.dir, collider.collider.ray.length).v2;
            auto vec = collider.collider.center.v2 + end;
            Draw::DebugLine(collider.collider.center, Vec(vec.x, vec.y, collider.collider.center.v3.z),
                collider.color, 2.0f);
        }
        break;
        case ColliderType::Sector:
        {
            Draw::DebugSector(collider.collider.center, collider.collider.sector.half_angle,
                collider.collider.ray.dir, collider.collider.ray.length, collider.color, 2.0f);
        }
            break;
        default:
        { }
        break;
        }
    }
}
