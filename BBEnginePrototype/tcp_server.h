#pragma once

#include "tcp_session.h"
#include "logger.h"

class TcpServer
{
public:
    TcpServer(uint16_t port)
        :acceptor_(io_context_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
    {
        Accept();
    }

    virtual ~TcpServer()
    {
        Stop();
    }

    virtual bool Start()
    {
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

    void Stop()
    {
        io_context_.stop();
        main_context_.wait();
        Logger::DebugLog("[SERVER] Stopped");
    }

    void Accept()
    {
        acceptor_.async_accept([this](const asio::error_code& error,
            asio::ip::tcp::socket socket)
            {
                HandleAccept(error, std::move(socket));
            });
    }

    virtual void HandleAccept(const asio::error_code& error,
        asio::ip::tcp::socket socket)
    {
        if (!error)
        {
            Logger::DebugLog("[SERVER] Accept New Connection : ");
        }
        else
        {
            Logger::DebugErr(error.message());
        }
        Accept();
    }

protected:
    asio::io_context io_context_;
    std::future<void> main_context_;
    asio::ip::tcp::acceptor acceptor_;
};