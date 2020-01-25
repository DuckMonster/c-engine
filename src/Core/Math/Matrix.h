#pragma once
#include "Vector.h"

struct Quat;

struct Mat4
{
	// Identity
	Mat4() :
	m00(1.f), m01(0.f), m02(0.f), m03(0.f),
	m10(0.f), m11(1.f), m12(0.f), m13(0.f),
	m20(0.f), m21(0.f), m22(1.f), m23(0.f),
	m30(0.f), m31(0.f), m32(0.f), m33(1.f) {}

	// Set everything!
	Mat4(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33
	) :
	m00(m00), m01(m01), m02(m02), m03(m03),
	m10(m10), m11(m11), m12(m12), m13(m13),
	m20(m20), m21(m21), m22(m22), m23(m23),
	m30(m30), m31(m31), m32(m32), m33(m33) {}

	// Diagonal setter
	Mat4(
		float m00, float m11, float m22, float m33
	) :
	m00(m00), m01(0.f), m02(0.f), m03(0.f),
	m10(0.f), m11(m11), m12(0.f), m13(0.f),
	m20(0.f), m21(0.f), m22(m22), m23(0.f),
	m30(0.f), m31(0.f), m32(0.f), m33(m33) {}

	float m00, m01, m02, m03;
	float m10, m11, m12, m13;
	float m20, m21, m22, m23;
	float m30, m31, m32, m33;

	// Returns the n-th row (basis vector) from a matrix
	Vec4& operator[](u32 row)
	{
		return ((Vec4*)this)[row];
	}
	const Vec4& operator[](u32 row) const
	{
		return ((Vec4*)this)[row];
	}
};

Mat4 operator*(const Mat4& a, const Mat4& b);
Vec4 operator*(const Mat4& m, const Vec4& v);
Vec3 operator*(const Mat4& m, const Vec3& v);

Mat4 mat_ortho(float left, float right, float bottom, float top, float near, float far);
Mat4 mat_perspective(float fov, float aspect, float near, float far);
Mat4 mat_look_at(const Vec3& eye, const Vec3& target, const Vec3& up);
Mat4 mat_look_forward(const Vec3& eye, const Vec3& forward, const Vec3& up);
Mat4 inverse(const Mat4& mat);
Mat4 transpose(const Mat4& mat);
void decompose(const Mat4& mat, Vec3* translation, Quat* rotation, Vec3* scale);

Mat4 mat_position(const Vec3& position);
Mat4 mat_position_rotation(const Vec3& position, const Quat& rotation);
Mat4 mat_position_scale(const Vec3& position, const Vec3& scale);
Mat4 mat_position_rotation_scale(const Vec3& position, const Quat& rotation, const Vec3& scale);