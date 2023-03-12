#pragma once

#include <vector>

struct PacketHeader
{
    uint16_t id = 0;
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
    ReadBuffer(uint16_t id, std::vector<uint8_t> buffer)
    {
        id_ = id;
        body_ = buffer;
    }

    uint16_t id_;
    std::vector<uint8_t> body_;
};