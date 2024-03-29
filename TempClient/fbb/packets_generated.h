// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_PACKETS_H_
#define FLATBUFFERS_GENERATED_PACKETS_H_

#include "flatbuffers/flatbuffers.h"

enum class PacketId : uint16_t {
  ChatReq = 0,
  ChatAck = 1,
  ChatNfy = 2,
  EnterNeighborsNfy = 3,
  LeaveNeighborsNfy = 4,
  UpdateNeighborsNfy = 5,
  MoveReq = 6,
  MoveAck = 7,
  MoveNfy = 8,
  DashReq = 9,
  DashAck = 10,
  DashNfy = 11,
  EnterWorldReq = 12,
  EnterWorldAck = 13,
  Debug3DPosition = 14,
  DebugColliderReq = 15,
  DebugColliderNfy = 16,
  MIN = ChatReq,
  MAX = DebugColliderNfy
};

inline const PacketId (&EnumValuesPacketId())[17] {
  static const PacketId values[] = {
    PacketId::ChatReq,
    PacketId::ChatAck,
    PacketId::ChatNfy,
    PacketId::EnterNeighborsNfy,
    PacketId::LeaveNeighborsNfy,
    PacketId::UpdateNeighborsNfy,
    PacketId::MoveReq,
    PacketId::MoveAck,
    PacketId::MoveNfy,
    PacketId::DashReq,
    PacketId::DashAck,
    PacketId::DashNfy,
    PacketId::EnterWorldReq,
    PacketId::EnterWorldAck,
    PacketId::Debug3DPosition,
    PacketId::DebugColliderReq,
    PacketId::DebugColliderNfy
  };
  return values;
}

inline const char * const *EnumNamesPacketId() {
  static const char * const names[18] = {
    "ChatReq",
    "ChatAck",
    "ChatNfy",
    "EnterNeighborsNfy",
    "LeaveNeighborsNfy",
    "UpdateNeighborsNfy",
    "MoveReq",
    "MoveAck",
    "MoveNfy",
    "DashReq",
    "DashAck",
    "DashNfy",
    "EnterWorldReq",
    "EnterWorldAck",
    "Debug3DPosition",
    "DebugColliderReq",
    "DebugColliderNfy",
    nullptr
  };
  return names;
}

inline const char *EnumNamePacketId(PacketId e) {
  if (flatbuffers::IsOutRange(e, PacketId::ChatReq, PacketId::DebugColliderNfy)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNamesPacketId()[index];
}

#endif  // FLATBUFFERS_GENERATED_PACKETS_H_
