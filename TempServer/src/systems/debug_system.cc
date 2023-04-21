#include "debug_system.h"
#include "dynamic_tree_system.h"

#include "mgr/datatable_mgr.h"
#include "world/region.h"
#include "net/user.h"
#include "bb_collider.h"
#include "bb_math.h"

#include "fbb/debug_generated.h"


void Recv_DebugColliderReq(void* session, Vector<uint8_t>& data)
{
    User* user = reinterpret_cast<User*>(session);
    DEBUG_RETURN(user);

    auto req = flatbuffers::GetRoot<DebugColliderReq>(data.data());

    LOG_INFO("Recv DebugColliderReq: ");
    if (auto world = user->world(); world)
    {
        DebugSystem::CreateDebugColliderObject(*world, user->eid(), req->dir(), 
            req->type());
    }
}

Collider DebugSystem::CreateCollider(short dir, const Vec& center, uint8_t type)
{
    Collider collider;
    collider.center = center;

    auto& debug = DataTable::instance().debug();

    switch ((ColliderType)type)
    {
    case ColliderType::Circle:
    {
        collider.type = Circle;
        collider.circle.r = debug[type].half_length;
    }
    break;
    case ColliderType::Sector:
    {
        collider.type = Sector;
        collider.sector.dir = dir;
        collider.sector.half_angle = debug[type].half_angle;
        collider.sector.r = debug[type].half_length;
    }
    break;
    case ColliderType::AABB:
    {
        collider.type = AABB;
        collider.aabb.hx = debug[type].half_width;
        collider.aabb.hy = debug[type].half_length;
    }
        break;
    case ColliderType::OBB:
    {
        collider.type = OBB;
        collider.obb.dir = dir;
        collider.obb.hw = debug[type].half_width;
        collider.obb.hl = debug[type].half_length;
        collider.center.v2 += BBMath::DirLength(dir, collider.obb.hl).v2;
    }
        break;
    case ColliderType::Ray:
    {
        collider.type = Ray;
        collider.ray.dir = dir;
        collider.ray.length = debug[type].half_length;
    }
        break;
    }

    return collider;
}

void DebugSystem::CreateDebugColliderObject(class Region& region, 
    Entity eid, short dir, uint8_t type)
{
    if (region.Valid<Transform>(eid))
    {
        auto& tf = region.get<Transform>(eid);
        flatbuffers::FlatBufferBuilder fbb(64);
        DebugColliderNfyT nfy;
        nfy.collider = SerializeDebugCollider(tf.v, type, dir);
        fbb.Finish(DebugColliderNfy::Pack(fbb, &nfy));
        region.BroadcastInRange(tf.v, region.viewing_range(), fbb, (uint16_t)PacketId::DebugColliderNfy);
    }
}

Unique<DebugColliderInfoT> DebugSystem::SerializeDebugCollider(const Vec& center, uint8_t type, short dir)
{
    auto info = std::make_unique<DebugColliderInfoT>();
    auto col = CreateCollider(dir, center, type);
    info->center = VecToUnique<fbVec>(col.center);
    info->type = col.type;
    switch (col.type)
    {
    case ColliderType::Circle:
        info->data.resize(sizeof(CircleInfo));
        memmove(info->data.data(), &col.circle, sizeof(CircleInfo));
        break;
    case ColliderType::Sector:
        info->data.resize(sizeof(SectorInfo));
        memmove(info->data.data(), &col.sector, sizeof(SectorInfo));
        break;
    case ColliderType::AABB:
        info->data.resize(sizeof(AABBInfo));
        memmove(info->data.data(), &col.aabb, sizeof(AABBInfo));
        break;
    case ColliderType::OBB:
        info->data.resize(sizeof(OBBInfo));
        memmove(info->data.data(), &col.obb, sizeof(OBBInfo));
        break;
    case ColliderType::Ray:
        info->data.resize(sizeof(RayInfo));
        memmove(info->data.data(), &col.ray, sizeof(RayInfo));
        break;
    }

    return std::move(info);
}
