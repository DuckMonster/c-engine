#include "Matrix.h"

Mat4 operator*(const Mat4& a, const Mat4& b)
{
	return Mat4(
		// a[col 0] * b[row 0]
		a.m00 * b.m00 + a.m10 * b.m01 + a.m20 * b.m02 + a.m30 * b.m03,
		// a[col 1] * b[row 0]
		a.m01 * b.m00 + a.m11 * b.m01 + a.m21 * b.m02 + a.m31 * b.m03,
		// a[col 2] * b[row 0]
		a.m02 * b.m00 + a.m12 * b.m01 + a.m22 * b.m02 + a.m32 * b.m03,
		// a[col 3] * b[row 0]
		a.m03 * b.m00 + a.m13 * b.m01 + a.m23 * b.m02 + a.m33 * b.m03,

		// a[col 0] * b[row 1]
		a.m00 * b.m10 + a.m10 * b.m11 + a.m20 * b.m12 + a.m30 * b.m13,
		// a[col 1] * b[row 1]
		a.m01 * b.m10 + a.m11 * b.m11 + a.m21 * b.m12 + a.m31 * b.m13,
		// a[col 2] * b[row 1]
		a.m02 * b.m10 + a.m12 * b.m11 + a.m22 * b.m12 + a.m32 * b.m13,
		// a[col 3] * b[row 1]
		a.m03 * b.m10 + a.m13 * b.m11 + a.m23 * b.m12 + a.m33 * b.m13,

		// a[col 0] * b[row 2]
		a.m00 * b.m20 + a.m10 * b.m21 + a.m20 * b.m22 + a.m30 * b.m23,
		// a[col 1] * b[row 2]
		a.m01 * b.m20 + a.m11 * b.m21 + a.m21 * b.m22 + a.m31 * b.m23,
		// a[col 2] * b[row 2]
		a.m02 * b.m20 + a.m12 * b.m21 + a.m22 * b.m22 + a.m32 * b.m23,
		// a[col 3] * b[row 2]
		a.m03 * b.m20 + a.m13 * b.m21 + a.m23 * b.m22 + a.m33 * b.m23,

		// a[col 0] * b[row 3]
		a.m00 * b.m30 + a.m10 * b.m31 + a.m20 * b.m32 + a.m30 * b.m33,
		// a[col 1] * b[row 3]
		a.m01 * b.m30 + a.m11 * b.m31 + a.m21 * b.m32 + a.m31 * b.m33,
		// a[col 2] * b[row 3]
		a.m02 * b.m30 + a.m12 * b.m31 + a.m22 * b.m32 + a.m32 * b.m33,
		// a[col 3] * b[row 3]
		a.m03 * b.m30 + a.m13 * b.m31 + a.m23 * b.m32 + a.m33 * b.m33
	);
}

void mat_ortho(Mat4* out_mat, float left, float right, float bottom, float top, float near, float far)
{
	*out_mat = Mat4(
		2.f / (right - left),				0.f,								0.f,							0.f,
		0.f,								2.f / (top - bottom),				0.f,							0.f,
		0.f,								0.f,								-2.f / (far - near),			0.f,
		-((right + left) / (right - left)),	-((top + bottom) / (top - bottom)), -((far + near) / (far - near)), 1.f
	);
}

void mat_look_at(Mat4* out_mat, const Vec3& eye, const Vec3& target, const Vec3& up)
{
	mat_look_forward(out_mat, eye, target - eye, up);
}

void mat_look_forward(Mat4* out_mat, const Vec3& eye, const Vec3& forward, const Vec3& up)
{
	Vec3 z_axis = normalize(-forward);
	Vec3 x_axis = normalize(cross(up, z_axis));
	Vec3 y_axis = cross(z_axis, x_axis);

	*out_mat = Mat4(
		x_axis.x, y_axis.x,	z_axis.x, 0.f,
		x_axis.y, y_axis.y,	z_axis.y, 0.f,
		x_axis.z, y_axis.z,	z_axis.z, 0.f,
		-dot(eye, x_axis), -dot(eye, y_axis), -dot(eye, z_axis), 1.f
	);
}