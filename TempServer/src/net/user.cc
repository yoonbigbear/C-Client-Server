#include "user.h"

#include "world/region.h"
#include "systems/systems.h"

#include <fbb/packets_generated.h>


User::~User()
{
    if (world_)
    {
        if (!world_->valid(entt::entity(eid_)))
            return;
        world_->Leave(entt::entity(eid_));
    }
}

bool User::Initialize(Shared<ClientSession> session)
{
    session->Initialize(this);
    tcp_ = session;

    return true;
}

bool User::Disconnet()
{
    world_->Leave((entt::entity)eid_);
    return true;
}

void User::ReadPackets()
{
    auto list = tcp_->recv_buffer().Get();
    for (auto& e : list)
    {
        PacketHandler::instance().Invoke(e.first)(this, e.second);
    }
}

