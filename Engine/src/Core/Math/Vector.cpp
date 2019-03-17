#include "Vector.h"

Vec2 operator+(const Vec2& a, const Vec2& b) { return Vec2(a.x + b.x, a.y + b.y); }
Vec3 operator+(const Vec3& a, const Vec3& b) { return Vec3(a.x + b.x, a.y + b.y, a.z + b.z); }
Vec3 operator+(const Vec4& a, const Vec4& b) { return Vec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w); }

Vec2 operator-(const Vec2& a, const Vec2& b) { return Vec2(a.x - b.x, a.y - b.y); }
Vec3 operator-(const Vec3& a, const Vec3& b) { return Vec3(a.x - b.x, a.y - b.y, a.z - b.z); }
Vec3 operator-(const Vec4& a, const Vec4& b) { return Vec4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w); }

Vec2 operator*(const Vec2& a, float scalar) { return Vec2(a.x * scalar, a.y * scalar); }
Vec3 operator*(const Vec3& a, float scalar) { return Vec3(a.x * scalar, a.y * scalar, a.z * scalar); }
Vec4 operator*(const Vec4& a, float scalar) { return Vec4(a.x * scalar, a.y * scalar, a.z * scalar, a.w * scalar); }

Vec2 operator/(const Vec2& a, float scalar) { return Vec2(a.x / scalar, a.y / scalar); }
Vec3 operator/(const Vec3& a, float scalar) { return Vec3(a.x / scalar, a.y / scalar, a.z / scalar); }
Vec4 operator/(const Vec4& a, float scalar) { return Vec4(a.x / scalar, a.y / scalar, a.z / scalar, a.w / scalar); }

Vec2 operator+(const Vec2& a, float scalar) { return Vec2(a.x + scalar, a.y + scalar); }
Vec3 operator+(const Vec3& a, float scalar) { return Vec3(a.x + scalar, a.y + scalar, a.z + scalar); }
Vec4 operator+(const Vec4& a, float scalar) { return Vec4(a.x + scalar, a.y + scalar, a.z + scalar, a.w + scalar); }

Vec2 operator-(const Vec2& a, float scalar) { return Vec2(a.x - scalar, a.y - scalar); }
Vec3 operator-(const Vec3& a, float scalar) { return Vec3(a.x - scalar, a.y - scalar, a.z - scalar); }
Vec4 operator-(const Vec4& a, float scalar) { return Vec4(a.x - scalar, a.y - scalar, a.z - scalar, a.w - scalar); }

Vec2& operator+=(Vec2& a, const Vec2& b) { a.x += b.x; a.y += b.y; return a; }
Vec3& operator+=(Vec3& a, const Vec3& b) { a.x += b.x; a.y += b.y; a.z += b.z; return a; }
Vec4& operator+=(Vec4& a, const Vec4& b) { a.x += b.x; a.y += b.y; a.z += b.z; a.w += b.w; return a; }

Vec2& operator-=(Vec2& a, const Vec2& b) { a.x -= b.x; a.y -= b.y; return a; }
Vec3& operator-=(Vec3& a, const Vec3& b) { a.x -= b.x; a.y -= b.y; a.z -= b.z; return a; }
Vec4& operator-=(Vec4& a, const Vec4& b) { a.x -= b.x; a.y -= b.y; a.z -= b.z; a.w -= b.w; return a; }

Vec2& operator*=(Vec2& a, float scalar) { a.x *= scalar; a.y *= scalar; return a; }
Vec3& operator*=(Vec3& a, float scalar) { a.x *= scalar; a.y *= scalar; a.z *= scalar; return a; }
Vec4& operator*=(Vec4& a, float scalar) { a.x *= scalar; a.y *= scalar; a.z *= scalar; a.w *= scalar; return a; }

Vec2& operator/=(Vec2& a, float scalar) { a.x /= scalar; a.y /= scalar; return a; }
Vec3& operator/=(Vec3& a, float scalar) { a.x /= scalar; a.y /= scalar; a.z /= scalar; return a; }
Vec4& operator/=(Vec4& a, float scalar) { a.x /= scalar; a.y /= scalar; a.z /= scalar; a.w /= scalar; return a; }

Vec2& operator+=(Vec2& a, float scalar) { a.x += scalar; a.y += scalar; return a; }
Vec3& operator+=(Vec3& a, float scalar) { a.x += scalar; a.y += scalar; a.z += scalar; return a; }
Vec4& operator+=(Vec4& a, float scalar) { a.x += scalar; a.y += scalar; a.z += scalar; a.w += scalar; return a; }

Vec2& operator-=(Vec2& a, float scalar) { a.x -= scalar; a.y -= scalar; return a; }
Vec3& operator-=(Vec3& a, float scalar) { a.x -= scalar; a.y -= scalar; a.z -= scalar; return a; }
Vec4& operator-=(Vec4& a, float scalar) { a.x -= scalar; a.y -= scalar; a.z -= scalar; a.w -= scalar; return a; }