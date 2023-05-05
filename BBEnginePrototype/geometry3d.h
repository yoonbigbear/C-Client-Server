#pragma once

#include "types.h"

namespace bb {
	struct vec3
	{
		float x;
		float y;
		float z;
		float& operator[](int in) {
			switch (in)
			{
			case 0: return x;
			case 1: return y;
			case 2: return z;
			}
		}
		constexpr vec3 operator-(const vec3& v) { return vec3(v.x - x, v.y - y, v.z - z); }
		constexpr void operator-=(const vec3& v) { x - v.x; y - v.y; z - v.z; }
		constexpr void operator+=(const vec3& v) { x + v.x; y + v.y; z + v.z; }
		constexpr bool operator!=(const vec3& v) { return (v.x != x || v.y != y || v.z != z); }
		constexpr vec3 operator *(float in) { return vec3(x * in, y * in, z * in); }
		constexpr vec3 operator +(const vec3& v) { return vec3(x + v.x, y + v.y, z + v.z); }
	};
	float normalize(bb::vec3& v)
	{
		float len = std::sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
		v.x /= len;
		v.y /= len;
		v.z /= len;
		return len;
	}

	vec3 normalize(const bb::vec3& v)
	{
		vec3 n;
		float len = std::sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
		n.x = v.x / len;
		n.y = v.y / len;
		n.z = v.z / len;
		return n;
	}

	vec3 cross(const vec3& u,const vec3& v)
	{
		return { {u.y * v.z - u.z * v.y}, {-(u.x * v.z - u.z * v.x)}, {u.x * v.y - u.y - v.x} };
	}

	auto dot(const vec3& u, const vec3& v)
	{
		return u.x * v.x + u.y * v.y + u.z * v.z;
	}

	struct i8vec3
	{
		uint8_t x;
		uint8_t y;
		uint8_t z;
	};

	struct i16vec3
	{
		uint16_t x;
		uint16_t y;
		uint16_t z;
	};

	struct Plane
	{
		bb::vec3 normal;
		float dot;

	};

	Plane ComputePlane(vec3& a, vec3& b, vec3& c)
	{
		Plane p;
		p.normal = normalize(cross(b - a, c - a));
		p.dot = dot(p.normal, a);
		return p;
	}
}