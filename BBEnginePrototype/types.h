#pragma once

#include <memory>
#include <vector>
#include <queue>
#include <string>
#include <list>
#include <deque>
#include <map>
#include <set>
#include <array>
#include <unordered_map>
#include <unordered_set>

#pragma warning(push, 3)
#pragma warning(disable : 4715)

#define __unused [[maybe_unused]]

using Byte = int8_t;
using SByte = uint8_t;
using Int16 = int16_t;
using UInt16 = uint16_t;
using Int32 = int32_t;
using UInt32 = uint32_t;
using Int64 = int64_t;
using UInt64 = uint64_t;
using Single = float;
using Double = double;
using Boolean = bool;

using String = std::string;
using StringView = std::string_view;
template<typename T>
using Unique = std::unique_ptr<T>;
template<typename T>
using Shared = std::shared_ptr<T>;
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

#ifdef _BOX2D
union Vec
{
    Vec():v2(b2Vec2(0,0)) {}
    Vec(const b2Vec2& v) : v2(v) {}
    Vec(const b2Vec3& v) : v3(v) {}
    Vec(const float v[3]) :v3({v[0],v[1],v[2]}) {}
    Vec(const float x, const float y) :v2({ x,y }) {}
    Vec(const float x, const float y, const float z) : v3({ x,y,z }) {}
    float& operator[](const unsigned int i)
    { 
        switch (i)
        {
        case 0: return v3.x;
        case 1: return v3.y;
        case 2: return v3.z;
        }
    }
    const float& operator[](const unsigned int i) const
    {
        switch (i)
        {
        case 0: return v3.x;
        case 1: return v3.y;
        case 2: return v3.z;
        }
    }
    auto operator*(float len) { return len * v2; }

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
    Vec(const b2Vec2& v) : v2(v) {}
    Vec(const b2Vec3& v) : v3(v) {}
    Vec(const float v[3]) :v3{ (v[0],v[1],v[2]) } {}
    Vec(const float x, const float y) :v2({ x,y }) {}
    Vec(const float x, const float y, const float z) : v3({ x,y,z }) {}
    float& operator[](const unsigned int i) { return v3[i]; }
    const float& operator[](const unsigned int i) const { return v3[i]; }
    auto operator*(float len) { return std::array<float, 3>{len* v3[0], len* v3[1], len* v3[2]}; }
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
    Vec(const float v[3]) :v3{ v[0], v[1],v[2] } {}
    Vec(const float x, const float y) :v2{ x,y } {}
    Vec(const float x, const float y, const float z) :v3{ x,y,z } {}
    float& operator[](const unsigned int i) { return v3[i]; }
    const float& operator[](const unsigned int i) const { return v3[i]; }
    auto operator*(float len) { return std::array<float, 3>{len* v3[0], len* v3[1], len* v3[2]}; }

    float v2[2];
    float v3[3];
};
using Vec2 = float[2];
using Vec3 = float[3];
#endif
#pragma warning(pop)

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

inline auto VecToDt(const Vec& v)
{
    return std::array<float,3>{ v[0], v[2], v[1] };
}
inline Vec DtToVec(const float* dt)
{
    return Vec{ dt[0], dt[2], dt[1] };
}

#define TOENTITY(eid) entt::entity entity = static_cast<entt::entity>(eid);