#include "client_session.h"

#include "systems/system_packet_handler.h"

#include "fbb/packets_generated.h"

void ClientSession::Initialize()
{
    TcpSession::Initialize();

    Bind((uint16_t)PacketId::Chat_Req, ChatREQ);
    //Bind((uint16_t)PacketId::EnterWorld_Req, EnterWorldREQ);
    Bind((uint16_t)PacketId::Move_Req, MoveREQ);
}

bool ClientSession::Bind(uint16_t id, PacketFunc fn)
{
    return packet_handler_.Bind(id, fn);
}

void ClientSession::Send(uint16_t id, uint16_t size, uint8_t* buf)
{
    PacketHeader header{ id , size };
    std::vector<uint8_t> data(sizeof(PacketHeader) + size);
    memcpy(data.data(), &header, sizeof(PacketHeader));
    memcpy(data.data() + sizeof(PacketHeader), buf, size);
    TcpSession::Send(data);
}

void ClientSession::Send(std::vector<uint8_t>& buf)
{
    TcpSession::Send(buf);
}
