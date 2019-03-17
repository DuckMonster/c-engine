#pragma once
#include <math.h>

struct Vec2
{
	Vec2() : x(0.f), y(0.f) {}
	Vec2(float x, float y) : x(x), y(y) {}
	float x;
	float y;
};

struct Vec3
{
	Vec3() : x(0.f), y(0.f), z(0.f) {}
	Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
	Vec3(const Vec2& vec, float z) : x(vec.x), y(vec.y), z(z) {}
	float x;
	float y;
	float z;
};

struct Vec4
{
	Vec4() : x(0.f), y(0.f), z(0.f), w(0.f) {}
	Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	Vec4(const Vec2& vec, float z, float w) : x(vec.x), y(vec.y), z(z), w(w) {}
	Vec4(const Vec3& vec, float w) : x(vec.x), y(vec.y), z(vec.z), w(w) {}

	float x;
	float y;
	float z;
	float w;
};

// Operators
inline Vec2 operator+(const Vec2& a, const Vec2& b) { return Vec2(a.x + b.x, a.y + b.y); }
inline Vec3 operator+(const Vec3& a, const Vec3& b) { return Vec3(a.x + b.x, a.y + b.y, a.z + b.z); }
inline Vec4 operator+(const Vec4& a, const Vec4& b) { return Vec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w); }

inline Vec2 operator-(const Vec2& a, const Vec2& b) { return Vec2(a.x - b.x, a.y - b.y); }
inline Vec3 operator-(const Vec3& a, const Vec3& b) { return Vec3(a.x - b.x, a.y - b.y, a.z - b.z); }
inline Vec4 operator-(const Vec4& a, const Vec4& b) { return Vec4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w); }

inline Vec2 operator*(const Vec2& a, float scalar) { return Vec2(a.x * scalar, a.y * scalar); }
inline Vec3 operator*(const Vec3& a, float scalar) { return Vec3(a.x * scalar, a.y * scalar, a.z * scalar); }
inline Vec4 operator*(const Vec4& a, float scalar) { return Vec4(a.x * scalar, a.y * scalar, a.z * scalar, a.w * scalar); }

inline Vec2 operator/(const Vec2& a, float scalar) { return Vec2(a.x / scalar, a.y / scalar); }
inline Vec3 operator/(const Vec3& a, float scalar) { return Vec3(a.x / scalar, a.y / scalar, a.z / scalar); }
inline Vec4 operator/(const Vec4& a, float scalar) { return Vec4(a.x / scalar, a.y / scalar, a.z / scalar, a.w / scalar); }

inline Vec2 operator+(const Vec2& a, float scalar) { return Vec2(a.x + scalar, a.y + scalar); }
inline Vec3 operator+(const Vec3& a, float scalar) { return Vec3(a.x + scalar, a.y + scalar, a.z + scalar); }
inline Vec4 operator+(const Vec4& a, float scalar) { return Vec4(a.x + scalar, a.y + scalar, a.z + scalar, a.w + scalar); }

inline Vec2 operator-(const Vec2& a, float scalar) { return Vec2(a.x - scalar, a.y - scalar); }
inline Vec3 operator-(const Vec3& a, float scalar) { return Vec3(a.x - scalar, a.y - scalar, a.z - scalar); }
inline Vec4 operator-(const Vec4& a, float scalar) { return Vec4(a.x - scalar, a.y - scalar, a.z - scalar, a.w - scalar); }

inline Vec2& operator+=(Vec2& a, const Vec2& b) { a.x += b.x; a.y += b.y; return a; }
inline Vec3& operator+=(Vec3& a, const Vec3& b) { a.x += b.x; a.y += b.y; a.z += b.z; return a; }
inline Vec4& operator+=(Vec4& a, const Vec4& b) { a.x += b.x; a.y += b.y; a.z += b.z; a.w += b.w; return a; }

inline Vec2& operator-=(Vec2& a, const Vec2& b) { a.x -= b.x; a.y -= b.y; return a; }
inline Vec3& operator-=(Vec3& a, const Vec3& b) { a.x -= b.x; a.y -= b.y; a.z -= b.z; return a; }
inline Vec4& operator-=(Vec4& a, const Vec4& b) { a.x -= b.x; a.y -= b.y; a.z -= b.z; a.w -= b.w; return a; }

inline Vec2& operator*=(Vec2& a, float scalar) { a.x *= scalar; a.y *= scalar; return a; }
inline Vec3& operator*=(Vec3& a, float scalar) { a.x *= scalar; a.y *= scalar; a.z *= scalar; return a; }
inline Vec4& operator*=(Vec4& a, float scalar) { a.x *= scalar; a.y *= scalar; a.z *= scalar; a.w *= scalar; return a; }

inline Vec2& operator/=(Vec2& a, float scalar) { a.x /= scalar; a.y /= scalar; return a; }
inline Vec3& operator/=(Vec3& a, float scalar) { a.x /= scalar; a.y /= scalar; a.z /= scalar; return a; }
inline Vec4& operator/=(Vec4& a, float scalar) { a.x /= scalar; a.y /= scalar; a.z /= scalar; a.w /= scalar; return a; }

inline Vec2& operator+=(Vec2& a, float scalar) { a.x += scalar; a.y += scalar; return a; }
inline Vec3& operator+=(Vec3& a, float scalar) { a.x += scalar; a.y += scalar; a.z += scalar; return a; }
inline Vec4& operator+=(Vec4& a, float scalar) { a.x += scalar; a.y += scalar; a.z += scalar; a.w += scalar; return a; }

inline Vec2& operator-=(Vec2& a, float scalar) { a.x -= scalar; a.y -= scalar; return a; }
inline Vec3& operator-=(Vec3& a, float scalar) { a.x -= scalar; a.y -= scalar; a.z -= scalar; return a; }
inline Vec4& operator-=(Vec4& a, float scalar) { a.x -= scalar; a.y -= scalar; a.z -= scalar; a.w -= scalar; return a; }

// Unary operators
inline Vec2 operator-(const Vec2& v) { return Vec2(-v.x, -v.y); }
inline Vec3 operator-(const Vec3& v) { return Vec3(-v.x, -v.y, -v.z); }
inline Vec4 operator-(const Vec4& v) { return Vec4(-v.x, -v.y, -v.z, -v.w); }

inline Vec2 normalize(const Vec2& v)
{
	float len = sqrt(v.x * v.x + v.y * v.y);
	return v / len;
}
inline Vec3 normalize(const Vec3& v)
{
	float len = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	return v / len;
}
inline Vec4 normalize(const Vec4& v)
{
	float len = sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
	return v / len;
}

inline float length(const Vec2& v)
{
	return sqrt(v.x * v.x + v.y * v.y);
}
inline float length(const Vec3& v)
{
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}
inline float length(const Vec4& v)
{
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

inline float length_sqrd(const Vec2& v)
{
	return sqrt(v.x * v.x + v.y * v.y);
}
inline float length_sqrd(const Vec3& v)
{
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}
inline float length_sqrd(const Vec4& v)
{
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

inline float dot(const Vec2& a, const Vec2& b)
{
	return a.x * b.x + a.y * b.y;
}
inline float dot(const Vec3& a, const Vec3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}
inline float dot(const Vec4& a, const Vec4& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

inline float cross(const Vec2& a, const Vec2& b)
{
	return a.x * b.y - a.y * b.x;
}
inline Vec3 cross(const Vec3& a, const Vec3& b)
{
	return Vec3(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}

// Vector constants
const Vec2 Vec2_X = Vec2(1.f, 0.f);
const Vec2 Vec2_Y = Vec2(0.f, 1.f);
const Vec2 Vec2_Zero = Vec2(0.f, 0.f);

const Vec3 Vec3_X = Vec3(1.f, 0.f, 0.f);
const Vec3 Vec3_Y = Vec3(0.f, 1.f, 0.f);
const Vec3 Vec3_Z = Vec3(0.f, 0.f, 1.f);
const Vec3 Vec3_Zero = Vec3(0.f, 0.f, 0.f);

const Vec4 Vec4_X = Vec4(1.f, 0.f, 0.f, 0.f);
const Vec4 Vec4_Y = Vec4(0.f, 1.f, 0.f, 0.f);
const Vec4 Vec4_Z = Vec4(0.f, 0.f, 1.f, 0.f);
const Vec4 Vec4_W = Vec4(0.f, 0.f, 0.f, 1.f);
const Vec4 Vec4_Zero = Vec4(0.f, 0.f, 0.f, 0.f);