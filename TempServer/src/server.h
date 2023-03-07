#pragma once

#include "tcp_server.h"
#include "world/world.h"

class Server : public TcpServer
{
public:
    Server(uint16_t port) : TcpServer(port)
    {}

    virtual bool Start() override
    {
        world_.Start();
        try
        {
            main_context_ = std::async(std::launch::async, [this] {io_context_.run(); });
        }
        catch (std::exception& e)
        {
            Logger::DebugErr(e.what());
            return false;
        }
        return true;
    }

    virtual void HandleAccept(const asio::error_code& error,
        asio::ip::tcp::socket socket) override
    {
        if (!error)
        {
            Logger::DebugLog("[SERVER] Accept New Session. Total :");

            total_sessions_++;
            std::shared_ptr<ClientSession> new_connection =
                std::make_shared<ClientSession>(io_context_, std::move(socket));
            new_connection->Start();

            EnterQueue(new_connection);
        }
        else
        {
            Logger::DebugErr(error.message());
        }
        Accept();
    }

    void Update()
    {
        EnterWorld();

        world_.FixedUpdate();
        world_.Update(0.01f);
    }

    void EnterQueue(std::shared_ptr<ClientSession> session)
    {
        auto lg = std::lock_guard(lock_);
        enter_queue.emplace_back(session);
    }

    void EnterWorld()
    {
        auto lg = std::lock_guard(lock_);
        for (auto& e : enter_queue)
        {
            world_.Enter(e);
        }
        enter_queue.clear();
    }

private:
    std::mutex lock_;

    World world_;
    uint32_t total_sessions_ = 0;
    std::vector<std::shared_ptr<ClientSession>> enter_queue;

};