#pragma once
#include <math.h>

struct Vec3;
struct Vec4;

struct Vec2
{
	Vec2() : x(0.f), y(0.f) {}
	Vec2(float x, float y) : x(x), y(y) {}
	explicit Vec2(float v) : x(v), y(v) {}
	explicit inline Vec2(const Vec3& v);
	explicit inline Vec2(const Vec4& v);

	float x;
	float y;

	Vec2& operator=(const Vec2& other) { x = other.x; y = other.y; return *this; }
	float& operator[](u32 index) { return ((float*)this)[index]; }
	const float& operator[](u32 index) const { return ((float*)this)[index]; }
};

struct Vec3
{
	Vec3() : x(0.f), y(0.f), z(0.f) {}
	Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
	Vec3(const Vec2& vec, float z) : x(vec.x), y(vec.y), z(z) {}
	explicit Vec3(float v) : x(v), y(v), z(v) {}
	explicit inline Vec3(const Vec4& v);

	float x;
	float y;
	float z;

	Vec3& operator=(const Vec3& other) { x = other.x; y = other.y; z = other.z; return *this; }
	float& operator[](u32 index) { return ((float*)this)[index]; }
	const float& operator[](u32 index) const { return ((float*)this)[index]; }
};

struct Vec4
{
	Vec4() : x(0.f), y(0.f), z(0.f), w(0.f) {}
	Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	explicit Vec4(float v) : x(v), y(v), z(v), w(v) {}
	Vec4(const Vec2& vec, float z, float w) : x(vec.x), y(vec.y), z(z), w(w) {}
	Vec4(const Vec3& vec, float w) : x(vec.x), y(vec.y), z(vec.z), w(w) {}
	Vec4(const Vec4& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}

	float x;
	float y;
	float z;
	float w;

	Vec4& operator=(const Vec4& other) { x = other.x; y = other.y; z = other.z; w = other.w; return *this; }
	float& operator[](u32 index) { return ((float*)this)[index]; }
	const float& operator[](u32 index) const { return ((float*)this)[index]; }
};

// Vector constants
const Vec2 Vec2_X = Vec2(1.f, 0.f);
const Vec2 Vec2_Y = Vec2(0.f, 1.f);
const Vec2 Vec2_Zero = Vec2(0.f, 0.f);
const Vec2 Vec2_One = Vec2(1.f, 1.f);
const Vec2 Vec2_NaN = Vec2(NAN);

const Vec3 Vec3_X = Vec3(1.f, 0.f, 0.f);
const Vec3 Vec3_Y = Vec3(0.f, 1.f, 0.f);
const Vec3 Vec3_Z = Vec3(0.f, 0.f, 1.f);
const Vec3 Vec3_Zero = Vec3(0.f, 0.f, 0.f);
const Vec3 Vec3_One = Vec3(1.f, 1.f, 1.f);
const Vec3 Vec3_NaN = Vec3(NAN);

const Vec4 Vec4_X = Vec4(1.f, 0.f, 0.f, 0.f);
const Vec4 Vec4_Y = Vec4(0.f, 1.f, 0.f, 0.f);
const Vec4 Vec4_Z = Vec4(0.f, 0.f, 1.f, 0.f);
const Vec4 Vec4_W = Vec4(0.f, 0.f, 0.f, 1.f);
const Vec4 Vec4_Zero = Vec4(0.f, 0.f, 0.f, 0.f);
const Vec4 Vec4_One = Vec4(1.f, 1.f, 1.f, 1.f);
const Vec4 Vec4_NaN = Vec4(NAN);

// Conversion operators
inline Vec2::Vec2(const Vec3& v) : x(v.x), y(v.y) {}
inline Vec2::Vec2(const Vec4& v) : x(v.x), y(v.y) {}
inline Vec3::Vec3(const Vec4& v) : x(v.x), y(v.y), z(v.z) {}

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

inline Vec2 operator*(const Vec2& a, const Vec2& b) { return Vec2(a.x * b.x, a.y * b.y); }
inline Vec3 operator*(const Vec3& a, const Vec3& b) { return Vec3(a.x * b.x, a.y * b.y, a.z * b.z); }
inline Vec4 operator*(const Vec4& a, const Vec4& b) { return Vec4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w); }

inline Vec2 operator/(const Vec2& a, float scalar) { return Vec2(a.x / scalar, a.y / scalar); }
inline Vec3 operator/(const Vec3& a, float scalar) { return Vec3(a.x / scalar, a.y / scalar, a.z / scalar); }
inline Vec4 operator/(const Vec4& a, float scalar) { return Vec4(a.x / scalar, a.y / scalar, a.z / scalar, a.w / scalar); }

inline Vec2 operator/(const Vec2& a, const Vec2& b) { return Vec2(a.x / b.x, a.y / b.y); }
inline Vec3 operator/(const Vec3& a, const Vec3& b) { return Vec3(a.x / b.x, a.y / b.y, a.z / b.z); }
inline Vec4 operator/(const Vec4& a, const Vec4& b) { return Vec4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w); }

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

inline Vec2& operator*=(Vec2& a, const Vec2& b) { a.x *= b.x; a.y *= b.y; return a; }
inline Vec3& operator*=(Vec3& a, const Vec3& b) { a.x *= b.x; a.y *= b.y; a.z *= b.z; return a; }
inline Vec4& operator*=(Vec4& a, const Vec4& b) { a.x *= b.x; a.y *= b.y; a.z *= b.z; a.w *= b.w; return a; }

inline Vec2& operator/=(Vec2& a, float scalar) { a.x /= scalar; a.y /= scalar; return a; }
inline Vec3& operator/=(Vec3& a, float scalar) { a.x /= scalar; a.y /= scalar; a.z /= scalar; return a; }
inline Vec4& operator/=(Vec4& a, float scalar) { a.x /= scalar; a.y /= scalar; a.z /= scalar; a.w /= scalar; return a; }

inline Vec2& operator/=(Vec2& a, const Vec2& b) { a.x /= b.x; a.y /= b.y; return a; }
inline Vec3& operator/=(Vec3& a, const Vec3& b) { a.x /= b.x; a.y /= b.y; a.z /= b.z; return a; }
inline Vec4& operator/=(Vec4& a, const Vec4& b) { a.x /= b.x; a.y /= b.y; a.z /= b.z; a.w /= b.w; return a; }

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

inline bool is_nearly_equal(const Vec2& a, const Vec2& b, float margin = KINDA_SMALL_NUMBER)
{
	return
		is_nearly_equal(a.x, b.x, margin) &&
		is_nearly_equal(a.y, b.y, margin);
}
inline bool is_nearly_equal(const Vec3& a, const Vec3& b, float margin = KINDA_SMALL_NUMBER)
{
	return
		is_nearly_equal(a.x, b.x, margin) &&
		is_nearly_equal(a.y, b.y, margin) &&
		is_nearly_equal(a.z, b.z, margin);
}
inline bool is_nearly_equal(const Vec4& a, const Vec4& b, float margin = KINDA_SMALL_NUMBER)
{
	return
		is_nearly_equal(a.x, b.x, margin) &&
		is_nearly_equal(a.y, b.y, margin) &&
		is_nearly_equal(a.z, b.z, margin) &&
		is_nearly_equal(a.w, b.w, margin);
}

inline bool is_nearly_zero(const Vec2& v, float margin = KINDA_SMALL_NUMBER)
{
	return
		is_nearly_zero(v.x, margin) &&
		is_nearly_zero(v.y, margin);
}
inline bool is_nearly_zero(const Vec3& v, float margin = KINDA_SMALL_NUMBER)
{
	return
		is_nearly_zero(v.x, margin) &&
		is_nearly_zero(v.y, margin) &&
		is_nearly_zero(v.z, margin);
}
inline bool is_nearly_zero(const Vec4& v, float margin = KINDA_SMALL_NUMBER)
{
	return
		is_nearly_zero(v.x, margin) &&
		is_nearly_zero(v.y, margin) &&
		is_nearly_zero(v.z, margin) &&
		is_nearly_zero(v.w, margin);
}

inline Vec2 normalize(const Vec2& v)
{
	float len = sqrtf(v.x * v.x + v.y * v.y);
	return v / len;
}
inline Vec3 normalize(const Vec3& v)
{
	float len = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	return v / len;
}
inline Vec4 normalize(const Vec4& v)
{
	float len = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
	return v / len;
}
inline Vec2 normalize_safe(const Vec2& v)
{
	float len = sqrtf(v.x * v.x + v.y * v.y);
	if (is_nearly_zero(len))
		return Vec2_X;

	return v / len;
}
inline Vec3 normalize_safe(const Vec3& v)
{
	float len = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	if (is_nearly_zero(len))
		return Vec3_X;

	return v / len;
}
inline Vec4 normalize_safe(const Vec4& v)
{
	float len = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
	if (is_nearly_zero(len))
		return Vec4_X;

	return v / len;
}

inline float length(const Vec2& v)
{
	return sqrtf(v.x * v.x + v.y * v.y);
}
inline float length(const Vec3& v)
{
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}
inline float length(const Vec4& v)
{
	return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

inline float length_sqrd(const Vec2& v)
{
	return v.x * v.x + v.y * v.y;
}
inline float length_sqrd(const Vec3& v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z;
}
inline float length_sqrd(const Vec4& v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;
}

inline float distance(const Vec2& a, const Vec2& b)
{
	return length(b - a);
}
inline float distance(const Vec3& a, const Vec3& b)
{
	return length(b - a);
}
inline float distance(const Vec4& a, const Vec4& b)
{
	return length(b - a);
}

inline float distance_sqrd(const Vec2& a, const Vec2& b)
{
	return length_sqrd(b - a);
}
inline float distance_sqrd(const Vec3& a, const Vec3& b)
{
	return length_sqrd(b - a);
}
inline float distance_sqrd(const Vec4& a, const Vec4& b)
{
	return length_sqrd(b - a);
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

inline Vec2 constrain_to_plane(const Vec2& vec, const Vec2& plane)
{
	return vec - plane * dot(vec, plane);
}
inline Vec3 constrain_to_plane(const Vec3& vec, const Vec3& plane)
{
	return vec - plane * dot(vec, plane);
}
inline Vec2 constrain_to_direction(const Vec2& vec, const Vec2& direction)
{
	return direction * dot(vec, direction);
}
inline Vec3 constrain_to_direction(const Vec3& vec, const Vec3& direction)
{
	return direction * dot(vec, direction);
}

inline Vec2 reflect(const Vec2& vec, const Vec2& reflection_normal)
{
	Vec2 projected_vec = constrain_to_direction(vec, reflection_normal);
	return vec - projected_vec * 2.f;
}
inline Vec3 reflect(const Vec3& vec, const Vec3& reflection_normal)
{
	Vec3 projected_vec = constrain_to_direction(vec, reflection_normal);
	return vec - projected_vec * 2.f;
}

inline Vec2 lerp(const Vec2& a, const Vec2& b, float t) { return a + (b - a) * t; }
inline Vec3 lerp(const Vec3& a, const Vec3& b, float t) { return a + (b - a) * t; }
inline Vec4 lerp(const Vec4& a, const Vec4& b, float t) { return a + (b - a) * t; }
inline Vec2 interp_constant(const Vec2& from, const Vec2& to, float speed, float delta_time)
{
	if (is_nearly_equal(from, to))
		return to;

	Vec2 dif = to - from;
	if (length_sqrd(dif) > square(speed * delta_time))
		dif = normalize(dif) * speed * delta_time;

	return from + dif;
}
inline Vec3 interp_constant(const Vec3& from, const Vec3& to, float speed, float delta_time)
{
	if (is_nearly_equal(from, to))
		return to;

	Vec3 dif = to - from;
	if (length_sqrd(dif) > square(speed * delta_time))
		dif = normalize(dif) * speed * delta_time;

	return from + dif;
}
inline Vec4 interp_constant(const Vec4& from, const Vec4& to, float speed, float delta_time)
{
	if (is_nearly_equal(from, to))
		return to;

	Vec4 dif = to - from;
	if (length_sqrd(dif) > square(speed * delta_time))
		dif = normalize(dif) * speed * delta_time;

	return from + dif;
}

inline bool is_nan(const Vec2& v) { return is_nan(v.x) || is_nan(v.y); }
inline bool is_nan(const Vec3& v) { return is_nan(v.x) || is_nan(v.y) || is_nan(v.z); }
inline bool is_nan(const Vec4& v) { return is_nan(v.x) || is_nan(v.y) || is_nan(v.z) || is_nan(v.w); }

inline bool is_counter_clockwise(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& normal)
{
	Vec3 first = b - a;
	Vec3 second = c - b;

	return dot(cross(first, second), normal) > 0.f;
}

inline Vec3 triangle_center(const Vec3& a, const Vec3& b, const Vec3& c)
{
	// The center will be two-thirds of the way from any vertex to the
	//	middle of the opposite edge
	// https://www.dummies.com/education/math/trigonometry/how-to-pinpoint-the-center-of-a-triangle/
	Vec3 edge_middle = lerp(a, b, 0.5f);
	return c + (edge_middle - c) * (2.f / 3.f);
}

inline Vec2 component_min(const Vec2& a, const Vec2& b)
{
	return Vec2(
		a.x < b.x ? a.x : b.x,
		a.y < b.y ? a.y : b.y
	);
}
inline Vec3 component_min(const Vec3& a, const Vec3& b)
{
	return Vec3(
		a.x < b.x ? a.x : b.x,
		a.y < b.y ? a.y : b.y,
		a.z < b.z ? a.z : b.z
	);
}
inline Vec4 component_min(const Vec4& a, const Vec4& b)
{
	return Vec4(
		a.x < b.x ? a.x : b.x,
		a.y < b.y ? a.y : b.y,
		a.z < b.z ? a.z : b.z,
		a.w < b.w ? a.w : b.w
	);
}
inline Vec2 component_max(const Vec2& a, const Vec2& b)
{
	return Vec2(
		a.x > b.x ? a.x : b.x,
		a.y > b.y ? a.y : b.y
	);
}
inline Vec3 component_max(const Vec3& a, const Vec3& b)
{
	return Vec3(
		a.x > b.x ? a.x : b.x,
		a.y > b.y ? a.y : b.y,
		a.z > b.z ? a.z : b.z
	);
}
inline Vec4 component_max(const Vec4& a, const Vec4& b)
{
	return Vec4(
		a.x > b.x ? a.x : b.x,
		a.y > b.y ? a.y : b.y,
		a.z > b.z ? a.z : b.z,
		a.w > b.w ? a.w : b.w
	);
}

inline Vec2 rotate_vector(const Vec2& vec, float angle)
{
	return Vec2(
		cos(angle) * vec.x - sin(angle) * vec.y,
		sin(angle) * vec.x + cos(angle) * vec.y
	);
}