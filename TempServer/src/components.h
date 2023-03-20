#pragma once

#include "pre.h"
#include "share/ecs/share_component.h"
#include "net/client_session.h"
#include "fbb/common_generated.h"

struct NetComponent
{
    Shared<ClientSession> session;
};

