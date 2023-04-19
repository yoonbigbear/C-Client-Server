#pragma once

#include "pre.h"

struct NetComponent
{
    Shared<class User> user;
};

class UserSystem
{
public:
    static Entity Construct(class Region& region, Shared<User> user);
    static void Send_EnterNeighborsAck(class Region& world, entt::entity caller);
};