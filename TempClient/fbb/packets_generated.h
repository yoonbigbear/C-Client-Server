// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_PACKETS_H_
#define FLATBUFFERS_GENERATED_PACKETS_H_

#include "flatbuffers/flatbuffers.h"

enum class PacketId : int16_t {
  Chat_Req = 0,
  Chat_Resp = 1,
  Chat_Sync = 2,
  EnterSync = 3,
  LeaveSync = 4,
  Move_Req = 5,
  Move_Resp = 6,
  Move_Sync = 7,
  EnterWorld_Req = 8,
  EnterWorld_Resp = 9,
  MIN = Chat_Req,
  MAX = EnterWorld_Resp
};

inline const PacketId (&EnumValuesPacketId())[10] {
  static const PacketId values[] = {
    PacketId::Chat_Req,
    PacketId::Chat_Resp,
    PacketId::Chat_Sync,
    PacketId::EnterSync,
    PacketId::LeaveSync,
    PacketId::Move_Req,
    PacketId::Move_Resp,
    PacketId::Move_Sync,
    PacketId::EnterWorld_Req,
    PacketId::EnterWorld_Resp
  };
  return values;
}

inline const char * const *EnumNamesPacketId() {
  static const char * const names[11] = {
    "Chat_Req",
    "Chat_Resp",
    "Chat_Sync",
    "EnterSync",
    "LeaveSync",
    "Move_Req",
    "Move_Resp",
    "Move_Sync",
    "EnterWorld_Req",
    "EnterWorld_Resp",
    nullptr
  };
  return names;
}

inline const char *EnumNamePacketId(PacketId e) {
  if (flatbuffers::IsOutRange(e, PacketId::Chat_Req, PacketId::EnterWorld_Resp)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNamesPacketId()[index];
}

#endif  // FLATBUFFERS_GENERATED_PACKETS_H_
