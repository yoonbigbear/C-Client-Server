#include "server.h"

#include "logger.h"

bool Server::Initialize()
{
    world_.Initialize();

    TcpServer::Initialize();

    return true;
}

void Server::HandleAccept(const asio::error_code& error, asio::ip::tcp::socket socket)
{
    if (!error)
    {
        LOG_INFO("[SERVER] Accept New Session. Total :");

        total_sessions_++;
        std::shared_ptr<ClientSession> new_connection =
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

void Server::Update()
{
    EnterWorld();

    world_.FixedUpdate();
    world_.Update(0.01f);
}

void Server::EnterQueue(std::shared_ptr<ClientSession> session)
{
    auto lg = std::lock_guard(lock_);
    enter_queue.emplace_back(session);
}

void Server::EnterWorld()
{
    auto lg = std::lock_guard(lock_);
    for (auto& e : enter_queue)
    {
        world_.Enter(e);
    }
    enter_queue.clear();
}
