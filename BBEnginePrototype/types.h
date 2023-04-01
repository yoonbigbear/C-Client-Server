#pragma once

#include <memory>
#include <vector>
#include <queue>
#include <string>
#include <list>
#include <deque>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>

#ifdef _BOX2D
union Vec
{
    Vec():v2(b2Vec2(0,0)) {}
    Vec(b2Vec2 v) : v2(v) {}
    Vec(b2Vec3 v) : v3(v) {}
    Vec(float v[3]) { v3.x = v[0]; v3.y = v[1]; v3.z = v[2]; }
    Vec(float x, float y) { v2.x = x; v2.y = y; }
    Vec(float x, float y, float z) { v3.x = x; v3.y = y; v3.z = z; }
    b2Vec2 v2;
    b2Vec3 v3;
};
using Vec2 = b2Vec2;
using Vec3 = b2Vec3;
using AABB2 = b2AABB;
#elif _BBENGINE
using Vec = union
{
    Vec() {}
    Vec(Vector2 v) : v2(v) {}
    Vec(Vector3 v) : v3(v) {}
    Vec(float v[3]) { v3.x = v[0]; v3.y = v[1]; v3.z = v[2]; }
    Vec(float x, float y) { v2.x = x; v2.y = y; }
    Vec(float x, float y, float z) { v3.x = x; v3.y = y; v3.z = z; }
    Vector2 v2;
    Vector3 v3;
};
using Vec2 = Vector2;
using Vec3 = Vector3;

#else
union Vec
{
    Vec() {}
    //vec(float v[2]) { v2[0] = v[0]; v2[1] = v[1]; }
    Vec(float v[3]) { v3[0] = v[0]; v3[1] = v[1]; v3[2] = v[2]; }
    Vec(float x, float y) { v2[0] = x; v2[1] = y; }
    Vec(float x, float y, float z) { v3[0] = x; v3[1] = y; v3[2] = z; }
    float v2[2];
    float v3[3];
};
using Vec2 = float[2];
using Vec3 = float[3];
struct AABB2 {
    Vec2 lower;
    Vec2 upper;
} AABB2;
#endif

#define __unused [[maybe_unused]]

template<typename T>
using Unique = std::unique_ptr<T>;
template<typename T>
using Shared = std::shared_ptr<T>;


using String = std::string;

template<typename T>
using Vector = std::vector<T>;
template<typename _Key, typename _Val>
using UnorderedMap = std::unordered_map<_Key, _Val>;
template<typename _Key>
using UnorderedSet = std::unordered_set<_Key>;
template<typename T>
using Set = std::set<T>;
template<typename _Key, typename _Val>
using Map = std::map<_Key, _Val>;
template<typename T>
using Queue = std::queue<T>;
template<typename T>
using List = std::list<T>;
template<typename T>
using Deque = std::deque<T>;

template<typename T>
inline T VecTo(const Vec& v)
{
    return T(v.v3.x, v.v3.y, v.v3.z);
}

template<typename T>
inline std::unique_ptr<T> VecToUnique(const Vec& v)
{
    return std::make_unique<T>(v.v3.x, v.v3.y, v.v3.z);
}
template<typename T>
inline std::shared_ptr<T> VecToShared(const Vec& v)
{
    return std::make_shared<T>(v.v3.x, v.v3.y, v.v3.z);
}
template<typename _In, typename _Out>
inline _Out VecTo(const _In& v)
{
    return _Out(v.x(), v.y(), v.z());
}
template<typename _In, typename _Out>
inline std::unique_ptr<_Out> VecToUnique(const _In& v)
{
    return std::make_unique<_Out>(v.x(), v.y(), v.z());
}
template<typename _In, typename _Out>
inline std::shared_ptr<_Out> VecToShared(const _In& v)
{
    return std::make_shared<_Out>(v.x(), v.y(), v.z());
}

#define TOENTITY(eid) entt::entity entity = static_cast<entt::entity>(eid);