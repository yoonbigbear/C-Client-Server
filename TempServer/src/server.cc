#include "server.h"

#include "logger.h"
#include "world/region.h"

#include "packet_handler.h"
#include "fbb/packets_generated.h"
#include "systems/systems.h"

bool Server::Initialize()
{
    world_ = std::make_shared<Region>();
    if (!world_->Initialize())
        return false;

    TcpServer::Initialize();

    BINDPACKET(EnterWorldReq);
    BINDPACKET(MoveReq);

    return true;
}

void Server::HandleAccept(const asio::error_code& error, asio::ip::tcp::socket socket)
{
    if (!error)
    {
        LOG_INFO("Accept New Session. Total :");

        Shared<User> new_user = std::make_shared<User>();
        new_user->Initialize(std::make_shared<ClientSession>(io_context_, std::move(socket)));
        EnterQueue(new_user);
    }
    else
    {
        LOG_ERROR("Exception: %s", error.message());
    }
    Accept();
}

void Server::Stop()
{
    TcpServer::Stop();
}

void Server::Update(float dt)
{
    EnterWorld();

    world_->Update(dt);
}

void Server::EnterQueue(Shared<User> session)
{
    auto lg = std::lock_guard(lock_);
    enter_queue.emplace_back(session);
}

void Server::EnterWorld()
{
    auto lg = std::lock_guard(lock_);
    for (auto& e : enter_queue)
    {
        e->eid((uint32_t)world_->EnterPlayer(e));
    }
    enter_queue.clear();
}
