#pragma once

#define WIN32_LEAN_AND_MEAN 
#define _ECS
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
#include <box2d/b2_dynamic_tree.h>
#pragma comment(lib, "box2d.lib")
#else
#include <bb_math.h>
#endif //  _BOX2D

#ifdef _ECS
#include <ecs.h>
#else
using Entity = uint32_t;
#endif


#include "bb_logger.h"
#include "filesystem.h"
#include "random_generator.h"
#include "types.h"

//assert check
#define  DEBUG_RETURN_VALUE(expr, ret) if(expr){ LOG_ERROR(#expr); return ret;};
#define  DEBUG_RETURN(expr) if(!expr){ LOG_ERROR(#expr); return;};
#define  DEBUG_ERROR(expr) if(expr) { LOG_ERROR(#expr);};

#ifdef DEBUG

#endif // DEBUG

