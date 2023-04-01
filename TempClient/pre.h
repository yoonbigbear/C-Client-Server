#pragma once

#define _CLIENT
#define _ASIO
#define _BOX2D

#pragma comment(lib, "BBEnginePrototype.lib")

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#pragma comment(lib, "glu32.lib")

#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <unordered_map>
#include <memory>
#include <string>

#include <entt/entt.hpp>

#ifdef _ASIO
#include <asio.hpp>
#endif

#ifdef _BOX2D
#include <box2d/box2d.h>
#include <box2d/b2_math.h>
#include <box2d/b2_collision.h>
#endif

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "SDL.h"
#include <crtdbg.h>

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

#ifdef __APPLE__
#	include <OpenGL/glu.h>
#else
#	include <GL/glu.h>
#endif

#include "gui.h"
#include "singleton.h"
#include "bb_util.h"
#include "bb_lock.h"
#include "draw.h"
#include "graphics.h"
#include "camera.h"

#include "types.h"
