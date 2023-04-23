#pragma once

#include "pre.h"

struct NpcComponent
{
    int tid;
};

class NpcSystem
{
public:
    static Entity CreateNpc(class Region& region, int tid);
};