#pragma once

#include "pre.h"

void RecvChatReq(void* session, std::vector<uint8_t>& data);

std::vector<uint8_t> BroadcastChat(const std::string& str);