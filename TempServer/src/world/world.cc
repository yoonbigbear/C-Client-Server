#include "world.h"
#include "src/systems.h"

#include "fbb/chat_generated.h"
#include "fbb/packets_generated.h"
#include "fbb/world_generated.h"

void World::Start()
{
    packet_handler_.Bind((unsigned short)PacketId::Chat_Req, ChatREQ);

    SpawnAI();
}

void World::Enter(std::shared_ptr<ClientSession> session)
{
    auto entity = registry_.create();

    registry_.emplace<NetComponent>(entity, session);

    //start pos
    auto& pos_comp = registry_.emplace<PositionComponent>(entity);

    // entity info
    auto& entity_data = registry_.emplace<EntityData>(entity);
    entity_data.eid = (std::uint32_t)entity;
    entity_data.flag = EntityFlag::Player;
    entity_data.tid = 1;

    //box collider
    auto& box_comp = registry_.emplace<AABBComponent>(entity);
    auto half_box = b2Vec2(5.f, 5.f);
    box_comp.box.lowerBound = pos_comp.pos - half_box;
    box_comp.box.upperBound = pos_comp.pos + half_box;

    //field
    //field_.Spawn(pos_comp.pos, box_comp.box, (void*)&entity_data);
    
    //sight
    auto& sight_comp = registry_.emplace<SightComponent>(entity);
    sight_comp.range = 500.f;


    /*{
        auto sight_half = sight_comp.range * 0.5f;
        auto sight = b2AABB();
        sight.lowerBound.x = pos_comp.pos.x - sight_half;
        sight.lowerBound.y = pos_comp.pos.y - sight_half;
        sight.upperBound.x = pos_comp.pos.x + sight_half;
        sight.upperBound.y = pos_comp.pos.y + sight_half;

        auto targets = field_.Query(sight);
        flatbuffers::FlatBufferBuilder fbb(256);
        SightSyncT sync;
        for (auto target : targets)
        {
            EntityInfo* entity_info = (EntityInfo*)target;

            sync.entities.push_back(EntityInfo{
                entity_info->pos(),
                entity_info->flag(),
                entity_info->table_id(),
                entity_info->entity_id()
                });
        }

    }*/

    printf("enter world %d\n", entity);
}

void World::Enter(int npcid)
{
    auto entity = registry_.create();

    // entity info
    auto& entity_data = registry_.emplace<EntityData>(entity);
    entity_data.eid = (std::uint32_t)entity;
    entity_data.flag = EntityFlag::Player;
    entity_data.tid = 1;

    // pos
    auto& pos_comp = registry_.emplace<PositionComponent>(entity);

    // collider
    auto& box_comp = registry_.emplace<AABBComponent>(entity);
    auto half_box = b2Vec2(5.f, 5.f);
    box_comp.box.lowerBound = pos_comp.pos - half_box;
    box_comp.box.upperBound = pos_comp.pos + half_box;

    //field
    //field_.Spawn(pos_comp.pos, box_comp.box, (void*)&entity_data);

    registry_.emplace<NpcComponent>(entity, npcid);
    {
        auto& wander = registry_.emplace<WanderComponent>(entity);
        wander.range = 100;
        wander.spawn_pos = pos_comp.pos;
        wander.acc_time = 2;
    }

    auto& sight_comp = registry_.emplace<SightComponent>(entity);
    sight_comp.range = 500.f;

    printf("enter world %d\n", entity);

}

void World::FixedUpdate()
{
    PacketHandling(registry_, packet_handler_);
}

void World::Update(float dt)
{
    //ai
    Wander(registry_, dt);
    Move(registry_, dt);

    //Broadcast(BroadcastChat("broadcast from server"));
}


void World::SpawnAI()
{
    int aicount = 2;
    for (int i = 0; i < aicount; ++i)
    {
        Enter(1);
    }
}

void World::Broadcast(std::vector<uint8_t>&& data)
{
    auto view = registry_.view<NetComponent>();
    for (auto [entity, net] : view.each())
    {
        net.session->Send(data, data.size());
    }
}

void World::BroadcastInSight(std::vector<uint8_t>&& data)
{

}
