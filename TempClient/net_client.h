#pragma once

#include <iostream>
#include <vector>
#include <asio.hpp>

#include "imgui.h"

#include "server_session.h"

#include "packet_handler.h"
#include "fbb/chat_generated.h"
#include "fbb/packets_generated.h"

class ClientInterface
{
public:
    ClientInterface() {}

    virtual ~ClientInterface()
    {
        Disconnect();
    }

    void Start()
    {
        Bind((unsigned short)PacketId::Chat_Sync, [&](std::vector<uint8_t>& data) {
            //      auto req = flatbuffers::GetRoot<ChatReq>(data.data());
          //printf(req->chat()->c_str());
            });
    }

    bool Bind(uint16_t id, std::function<void(std::vector<uint8_t>&)> fn)
    {
        return packet_handler_.Bind(id, fn);
    }

    bool Connect(const std::string& host, const uint16_t port)
    {
        try
        {
            asio::ip::tcp::resolver resolver(io_context_);
            asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));

            connection_ = std::make_unique<ServerSession>(io_context_,
                asio::ip::tcp::socket(io_context_));
            connection_->ConnectToServer(endpoints);
            connection_->Start();
            thread = std::async(std::launch::async, [this]() {io_context_.run(); });
        }
        catch (std::exception& e)
        {
            std::cerr << "client exception: " << e.what() << std::endl;
            return false;
        }

        return true;
    }

    void Disconnect()
    {
        if (connection_)
        {
            connection_->Disconnect();
            connection_.release();
        }
        io_context_.stop();

        if (thread.valid())
            thread.wait();
    }

    void ReadPackets()
    {
        auto list = connection_->recv_buffer().Get();
        for (auto& e : list)
        {
            packet_handler_.Invoke(e.first)(e.second);
        }
    }

    void GuiLogin()
    {
        ImGui::Begin("Login Popup");

        if (ImGui::Button("ConnectServer"))
        {
            Connect("127.0.0.1", 11000);
        }
        ImGui::End();
    }

public:

    void Send(const char* buf, int size)
    {
        connection_->Send(buf, size);
    }

protected:
    asio::io_context io_context_;
    std::future<void> thread;
    std::unique_ptr<ServerSession> connection_;
    PacketHandler packet_handler_;
};
