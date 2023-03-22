#include "server.h"

#include "logger.h"
#include "world/region.h"

#include "packet_handler.h"
#include "fbb/packets_generated.h"
#include "systems/systems.h"

bool Server::Initialize()
{
    world_ = std::make_shared<Region>();
    world_->Initialize();

    TcpServer::Initialize();

    PacketHandler::instance().Bind((uint16_t)PacketId::Chat_Req, RecvChatReq);
    PacketHandler::instance().Bind((uint16_t)PacketId::EnterWorld_Req, EnterRegion);
    PacketHandler::instance().Bind((uint16_t)PacketId::Move_Req, RecvMoveReq);
    return true;
}

void Server::HandleAccept(const asio::error_code& error, asio::ip::tcp::socket socket)
{
    if (!error)
    {
        LOG_INFO("Accept New Session. Total :");

        total_sessions_++;
        Shared<ClientSession> new_connection =
            std::make_shared<ClientSession>(io_context_, std::move(socket));
        new_connection->Initialize();

        EnterQueue(new_connection);
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

void Server::EnterQueue(Shared<ClientSession> session)
{
    auto lg = std::lock_guard(lock_);
    enter_queue.emplace_back(session);
}

void Server::EnterWorld()
{
    auto lg = std::lock_guard(lock_);
    for (auto& e : enter_queue)
    {
        world_->Enter(e);
    }
    enter_queue.clear();
}
