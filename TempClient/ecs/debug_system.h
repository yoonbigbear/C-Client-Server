#pragma once

#include "pre.h"


void DrawDebugToggledObjects(Shared<class Region> scene);

void DrawDebugPosition(Shared<class Region> scene, Vec pos);

void Recv_Debug3DPosition(void* session, Vector<uint8_t>& data);
void Recv_DebugColliderNfy(void* session, Vector<uint8_t>& data);

struct CylinderData
{
    enum
    {
        max_climb = 1,
    };
    float height;
    float radius;
};

struct DebugCollider
{
    Collider collider;
    uint32_t color;
    float ttl;
};

class DebugSystem
{
public:
    static void SendDebugCollision(class Region& region, ColliderType type, short dir, entt::entity entity);
    static void CreateDebugColliderObject(class Region& region, Collider&& c, uint32_t col, float ttl = 0.5f);
    static void Update(class Region& scene, float dt);

private:
    static void UpdateDebugCollider(class Region& scene, float dt);
};