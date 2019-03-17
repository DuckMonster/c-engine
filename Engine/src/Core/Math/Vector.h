#pragma once

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
Vec2 operator+(const Vec2& a, const Vec2& b);
Vec3 operator+(const Vec3& a, const Vec3& b);
Vec4 operator+(const Vec4& a, const Vec4& b);

Vec2 operator-(const Vec2& a, const Vec2& b);
Vec3 operator-(const Vec3& a, const Vec3& b);
Vec4 operator-(const Vec4& a, const Vec4& b);

Vec2 operator*(const Vec2& a, float scalar);
Vec3 operator*(const Vec3& a, float scalar);
Vec4 operator*(const Vec4& a, float scalar);

Vec2 operator/(const Vec2& a, float scalar);
Vec3 operator/(const Vec3& a, float scalar);
Vec4 operator/(const Vec4& a, float scalar);

Vec2 operator+(const Vec2& a, float scalar);
Vec3 operator+(const Vec3& a, float scalar);
Vec4 operator+(const Vec4& a, float scalar);

Vec2 operator-(const Vec2& a, float scalar);
Vec3 operator-(const Vec3& a, float scalar);
Vec4 operator-(const Vec4& a, float scalar);

Vec2& operator+=(Vec2& a, const Vec2& b);
Vec3& operator+=(Vec3& a, const Vec3& b);
Vec4& operator+=(Vec4& a, const Vec4& b);

Vec2& operator-=(Vec2& a, const Vec2& b);
Vec3& operator-=(Vec3& a, const Vec3& b);
Vec4& operator-=(Vec4& a, const Vec4& b);

Vec2& operator*=(Vec2& a, float scalar);
Vec3& operator*=(Vec3& a, float scalar);
Vec4& operator*=(Vec4& a, float scalar);

Vec2& operator/=(Vec2& a, float scalar);
Vec3& operator/=(Vec3& a, float scalar);
Vec4& operator/=(Vec4& a, float scalar);

Vec2& operator+=(Vec2& a, float scalar);
Vec3& operator+=(Vec3& a, float scalar);
Vec4& operator+=(Vec4& a, float scalar);

Vec2& operator-=(Vec2& a, float scalar);
Vec3& operator-=(Vec3& a, float scalar);
Vec4& operator-=(Vec4& a, float scalar);

Vec2 normalize(const Vec2& v);
Vec3 normalize(const Vec3& v);
Vec4 normalize(const Vec4& v);

float length(const Vec2& v);
float length(const Vec3& v);
float length(const Vec4& v);

float length_sqrd(const Vec2& v);
float length_sqrd(const Vec3& v);
float length_sqrd(const Vec4& v);