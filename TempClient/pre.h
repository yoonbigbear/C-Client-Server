#pragma once

#define _ASIO
#define _BOX2D

#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <unordered_map>
#include <memory>
#include <string>

#ifdef _ASIO
#include <asio.hpp>
#endif

#ifdef _BOX2D
#include <box2d/box2d.h>
#include <box2d/b2_math.h>
#include <box2d/b2_collision.h>
#endif

#include "gui.h"
#include "singleton.h"
#include "bb_util.h"
#include "bb_lock.h"
#include "draw.h"

#include "types.h"
