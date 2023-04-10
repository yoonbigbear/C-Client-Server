#pragma once

#define _BBLOG

#include "packet_buffer.h"
#include "recv_buffer.h"
#include "bb_logger.h"

#include <asio.hpp>
#include <deque>
#include <memory>

using tcp = asio::ip::tcp;

class TcpSession
{
public:
    TcpSession(asio::io_context& io_context, tcp::socket socket)
        :io_context_(io_context)
        , socket_(std::move(socket))
    {
    }
    void Initialize()
    {
        socket_.set_option(asio::socket_base::keep_alive(true));
        socket_.set_option(tcp::no_delay(true));
        Read();
    }
    virtual void Disconnect()
    {
        if (IsConnected())
        {
            LOG_INFO("socket close");
            socket_.shutdown(socket_.shutdown_both);
            socket_.close();
        }
    }
    bool IsConnected() const
    {
        return socket_.is_open();
    }

protected:
    void Send(const char* buf, size_t size) 
    {
        asio::post(io_context_, [&, buf, size]() { Write(buf, size); });

    }
    void Send(const std::vector<uint8_t>& buf) 
    {
        asio::post(io_context_, [this, buf]() { Write(buf, buf.size()); });
    }

    void Write(const std::vector<uint8_t>& buf, size_t size) 
    {
        asio::async_write(socket_, asio::buffer(buf.data(), size),
            [this](asio::error_code ec, std::size_t /*length*/)
            {
                if (!ec)
                {

                }
                else
                {
                    /*room_.leave(shared_from_this());*/
                }
            });
    }
    void Write(const char* buf, size_t size)
    {
        asio::async_write(socket_, asio::buffer(buf, size),
            [this](asio::error_code ec, std::size_t /*length*/)
            {
                if (!ec)
                {

                }
                else
                {
                    /*room_.leave(shared_from_this());*/
                }
            });
    }
    void Read()
    {
        // asio::post(io_context_, [&]() {
        recv_buffer_.fill(0);
        socket_.async_receive(asio::buffer(recv_buffer_, 0xFFFF),
            [this](asio::error_code ec, std::size_t length)
            {
                if (!ec)
                {
                    if (length > 0)
                    {
                        try
                        {
                            PacketHeader header;
                            memmove(&header, recv_buffer_.data(), sizeof(PacketHeader));
                            std::vector<uint8_t>body_vec(header.size);
                            memmove(body_vec.data(), recv_buffer_.data() + sizeof(PacketHeader), header.size);
                            recv_packets_.Add(header.id, std::move(body_vec));
                            Read();
                        }
                        catch (std::exception& e)
                        {
                            throw e;
                            return;
                        }
                    }
                }
                else
                {
                    Disconnect();
                    return;
                }
            });
        //  });
    }

public:
    RecvBuffer& recv_buffer() { return recv_packets_; }

protected:
    tcp::socket socket_;
    RecvBuffer recv_packets_;
    std::array<uint8_t, 0xFFFF> recv_buffer_;
    asio::io_context& io_context_;
    std::deque<std::vector<uint8_t>> send_buffer_;
};