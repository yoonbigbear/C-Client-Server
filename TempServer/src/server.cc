#include "server.h"

#include "world/region.h"

#include "packet_handler.h"
#include "fbb/packets_generated.h"
#include "systems/systems.h"
#include "systems/user_system.h"

bool Server::Initialize()
{
    TcpServer::Initialize();

    world_ = std::make_shared<Region>();
    if (!world_->Initialize())
        return false;

    BINDPACKET(EnterWorldReq);
    BINDPACKET(MoveReq);

    return true;
}

void Server::HandleAccept(const asio::error_code& error, asio::ip::tcp::socket socket)
{
    if (!error)
    {
        LOG_INFO("Accept New Session. Total :");

        try 
        {
            Shared<User> new_user = std::make_shared<User>();
            new_user->Initialize(std::make_shared<ClientSession>(io_context_, std::move(socket)));
            EnterQueue(new_user);
        }
        catch (std::exception& e)
        {
            throw e;
        }
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
    LOG_INFO("[SERVER] Stopped");
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
        e->eid(static_cast<uint32_t>(UserSystem::Construct(*world_, e)));
    }
    enter_queue.clear();
}
