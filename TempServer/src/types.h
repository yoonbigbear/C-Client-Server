#pragma once

#ifdef _BOX2D
using vec2 = b2Vec2;
using vec3 = b2Vec3;
using AABB2 = b2AABB;
#elif _BBENGINE
using vec3 = Vector3;
using vec2 = Vector2;
#endif

template<typename T>
using Vector = std::vector<T>;