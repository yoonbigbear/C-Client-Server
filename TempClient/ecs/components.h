#pragma once

#include "pre.h"
#include "net_client.h"
#include "share/ecs/share_component.h"

struct CylinderData
{
    enum
    {
        max_climb = 1,
    };
    float height;
    float radius;
};

struct MyPlayer
{
    Shared<NetClient> session;
};