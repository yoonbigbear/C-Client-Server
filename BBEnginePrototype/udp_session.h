#pragma once

#include <asio.hpp>

using udp = asio::ip::udp;

class udp_server
{
public:
    udp_server(asio::io_context& io_context)
        : socket_(io_context, udp::endpoint(udp::v4(), 13))
    {
        start_receive();
    }

private:
    void start_receive()
    {
        socket_.async_receive_from(
            asio::buffer(recv_buffer_), remote_endpoint_,
            std::bind(&udp_server::handle_receive, this,
                asio::error_code()));
    }

    void handle_receive(const asio::error_code& error)
    {
        if (!error)
        {
            std::shared_ptr<std::string> message(
                new std::string());

            socket_.async_send_to(asio::buffer(*message), remote_endpoint_,
                std::bind(&udp_server::handle_send, this, message));

            start_receive();
        }
    }

    void handle_send(std::shared_ptr<std::string> /*message*/)
    {
    }

    udp::socket socket_;
    udp::endpoint remote_endpoint_;
    std::array<char, 1> recv_buffer_;
};