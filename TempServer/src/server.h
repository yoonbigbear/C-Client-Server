#pragma once

#include "pre.h"
#include "tcp_server.h"
#include "net/user.h"


class Region;
class Server : public TcpServer
{
public:
    Server(uint16_t port) : TcpServer(port) {}

    virtual bool Initialize() override final;

    void Update(float dt);

    void EnterQueue(Shared<User> session);
    void EnterWorld();
    
    virtual void HandleAccept(const asio::error_code& error,
        asio::ip::tcp::socket socket) override final;
    virtual void Stop() override final;

private:
    std::mutex lock_;

    Shared<Region> world_;
    Vector<Shared<User>> enter_queue;
    UnorderedSet<Shared<User>> TcpSessions;
};