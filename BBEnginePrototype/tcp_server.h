#pragma once

#include "tcp_session.h"

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

    virtual bool Initialize()
    {
        try
        {
            io_thread_ = std::async(std::launch::async, [this] {io_context_.run(); });
        }
        catch (std::exception& e)
        {
            throw e;
        }
        return true;
    }

    virtual void Stop()
    {
        io_context_.stop();
        if (io_thread_.valid())
            io_thread_.wait();
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
        }
        else
        {
        }
        Accept();
    }

protected:
    asio::io_context io_context_;
    std::future<void> io_thread_;
    asio::ip::tcp::acceptor acceptor_;
};