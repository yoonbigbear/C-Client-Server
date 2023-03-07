#pragma once

#include <vector>
#include <array>

struct PacketHeader
{
    short id = 0;
    uint16_t size = 0;
};
struct PacketBuffer
{
    PacketHeader header_;
    std::vector<uint8_t> body_;
};

class ReadBuffer
{
public:
    ReadBuffer(short id, std::vector<uint8_t> buffer)
    {
        id_ = id;
        body_ = buffer;
    }

    short id_;
    std::vector<uint8_t> body_;
};