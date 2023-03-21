#pragma once

#define WIN32_LEAN_AND_MEAN 

#define _BOX2D
#define _SPDLOG

#pragma warning(disable: 4819)
#pragma warning(disable:4996) //CRT_NO_WARNING

#include <vector>
#include <functional>
#include <filesystem>
#include <string>
#include <iostream>

#ifdef  _BOX2D
#include <box2d/b2_math.h>
#include <box2d/b2_collision.h>
#pragma comment(lib, "box2d.lib")
#else
#include <bb_math.h>
#endif //  _BOX2D

#include <entt/entt.hpp>

#include "logger.h"
#include "random_generator.h"
#include "types.h"

//assert check
#define  ASSERT_RETURN(expr, ret) if(expr){ LOG_ERROR(#expr); return ret;};
#define  ASSERT_ERROR(expr) if(expr) { LOG_ERROR(#expr);};

//// "Protocol_"À» »« ÆÐÅ¶id
//#define BUILD_PACKET(protocol, ...)   \
//net::Message<Protocol, flatbuffers::FlatBufferBuilder> pkt; \
//pkt.header.id = Protocol_##protocol##;  \
//flatbuffers::FlatBufferBuilder fbb(256);   \
//auto builder = account::Create##protocol##Direct(fbb, ##__VA_ARGS__);    \
//fbb.Finish(builder);    \
//pkt << fbb;
//
//#define BUILD_SIMPLE_PACKET(protocol, ...)   \
//net::Message<Protocol, flatbuffers::FlatBufferBuilder> pkt; \
//pkt.header.id = Protocol_##protocol##;  \
//flatbuffers::FlatBufferBuilder fbb(256);   \
//auto builder = account::Create##protocol##(fbb, ##__VA_ARGS__);    \
//fbb.Finish(builder);    \
//pkt << fbb;
//
//#define WAIT_UNTIL_BEGIN(protocol) \
//while (true) \
// { \
//  if (!c.Incoming().empty()) \
//   { \
//    auto msg = c.Incoming().pop_front(); \
//    if (msg.msg.header.id == protocol) \
//    {;
//
//#define WAIT_UNTIL_END \
//    }\
//   }\
// };