#pragma once

#include "tcp_server.h"
#include "world/world.h"
#include "net/client_session.h"

class Server : public TcpServer
{
public:
    Server(uint16_t port) : TcpServer(port) {}
    ~Server() override final
    {
        Stop();
    }

    virtual bool Initialize() override final;

    void Update(float dt);

    void EnterQueue(Shared<ClientSession> session);
    void EnterWorld();
    
    virtual void HandleAccept(const asio::error_code& error,
        asio::ip::tcp::socket socket) override final;
    virtual void Stop() override final;

private:
    std::mutex lock_;

    World world_;
    uint32_t total_sessions_ = 0;
    Vector<Shared<ClientSession>> enter_queue;

};