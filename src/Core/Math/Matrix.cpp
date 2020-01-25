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

Vec4 operator*(const Mat4& m, const Vec4& v)
{
	Vec4 result;

	result.x = m.m00 * v.x + m.m10 * v.y + m.m20 * v.z + m.m30 * v.w;
	result.y = m.m01 * v.x + m.m11 * v.y + m.m21 * v.z + m.m31 * v.w;
	result.z = m.m02 * v.x + m.m12 * v.y + m.m22 * v.z + m.m32 * v.w;
	result.w = m.m03 * v.x + m.m13 * v.y + m.m23 * v.z + m.m33 * v.w;

	return result;
}

Vec3 operator*(const Mat4& m, const Vec3& v)
{
	// Assumed W component as 1
	Vec3 result;

	result.x = m.m00 * v.x + m.m10 * v.y + m.m20 * v.z + m.m30;
	result.y = m.m01 * v.x + m.m11 * v.y + m.m21 * v.z + m.m31;
	result.z = m.m02 * v.x + m.m12 * v.y + m.m22 * v.z + m.m32;

	return result;
}

Mat4 mat_ortho(float left, float right, float bottom, float top, float near, float far)
{
	return Mat4(
		2.f / (right - left),				0.f,								0.f,							0.f,
		0.f,								2.f / (top - bottom),				0.f,							0.f,
		0.f,								0.f,								-2.f / (far - near),			0.f,
		-((right + left) / (right - left)),	-((top + bottom) / (top - bottom)), -((far + near) / (far - near)), 1.f
	);
}

Mat4 mat_perspective(float fov, float aspect, float near, float far)
{
	// Look this up :(
	const float tan_half_fov = tan(fov / 2.f);
	return Mat4(
		1.f / (aspect * tan_half_fov),					0.f, 0.f, 0.f,
		0.f, 1.f / tan_half_fov,						0.f, 0.f,
		0.f, 0.f, -((far + near) / (far - near)),		-1.f,
		0.f, 0.f, -((2.f * far * near) / (far - near)),	0.f
	);
}

Mat4 mat_look_at(const Vec3& eye, const Vec3& target, const Vec3& up)
{
	return mat_look_forward(eye, target - eye, up);
}

Mat4 mat_look_forward(const Vec3& eye, const Vec3& forward, const Vec3& up)
{
	Vec3 z_axis = normalize(-forward);
	Vec3 x_axis = normalize(cross(up, z_axis));
	Vec3 y_axis = cross(z_axis, x_axis);

	return Mat4(
		x_axis.x, y_axis.x,	z_axis.x, 0.f,
		x_axis.y, y_axis.y,	z_axis.y, 0.f,
		x_axis.z, y_axis.z,	z_axis.z, 0.f,
		-dot(eye, x_axis), -dot(eye, y_axis), -dot(eye, z_axis), 1.f
	);
}

Mat4 inverse(const Mat4& mat)
{
	float* src = (float*)&mat;
	// Copied straight from Unreal! Read up on this someday.
	const float s0  = (float)(src[ 0]); const float s1  = (float)(src[ 1]); const float s2  = (float)(src[ 2]); const float s3  = (float)(src[ 3]);
	const float s4  = (float)(src[ 4]); const float s5  = (float)(src[ 5]); const float s6  = (float)(src[ 6]); const float s7  = (float)(src[ 7]);
	const float s8  = (float)(src[ 8]); const float s9  = (float)(src[ 9]); const float s10 = (float)(src[10]); const float s11 = (float)(src[11]);
	const float s12 = (float)(src[12]); const float s13 = (float)(src[13]); const float s14 = (float)(src[14]); const float s15 = (float)(src[15]);

	float inv[16];
	inv[0]  =  s5 * s10 * s15 - s5 * s11 * s14 - s9 * s6 * s15 + s9 * s7 * s14 + s13 * s6 * s11 - s13 * s7 * s10;
	inv[1]  = -s1 * s10 * s15 + s1 * s11 * s14 + s9 * s2 * s15 - s9 * s3 * s14 - s13 * s2 * s11 + s13 * s3 * s10;
	inv[2]  =  s1 * s6  * s15 - s1 * s7  * s14 - s5 * s2 * s15 + s5 * s3 * s14 + s13 * s2 * s7  - s13 * s3 * s6;
	inv[3]  = -s1 * s6  * s11 + s1 * s7  * s10 + s5 * s2 * s11 - s5 * s3 * s10 - s9  * s2 * s7  + s9  * s3 * s6;
	inv[4]  = -s4 * s10 * s15 + s4 * s11 * s14 + s8 * s6 * s15 - s8 * s7 * s14 - s12 * s6 * s11 + s12 * s7 * s10;
	inv[5]  =  s0 * s10 * s15 - s0 * s11 * s14 - s8 * s2 * s15 + s8 * s3 * s14 + s12 * s2 * s11 - s12 * s3 * s10;
	inv[6]  = -s0 * s6  * s15 + s0 * s7  * s14 + s4 * s2 * s15 - s4 * s3 * s14 - s12 * s2 * s7  + s12 * s3 * s6;
	inv[7]  =  s0 * s6  * s11 - s0 * s7  * s10 - s4 * s2 * s11 + s4 * s3 * s10 + s8  * s2 * s7  - s8  * s3 * s6;
	inv[8]  =  s4 * s9  * s15 - s4 * s11 * s13 - s8 * s5 * s15 + s8 * s7 * s13 + s12 * s5 * s11 - s12 * s7 * s9;
	inv[9]  = -s0 * s9  * s15 + s0 * s11 * s13 + s8 * s1 * s15 - s8 * s3 * s13 - s12 * s1 * s11 + s12 * s3 * s9;
	inv[10] =  s0 * s5  * s15 - s0 * s7  * s13 - s4 * s1 * s15 + s4 * s3 * s13 + s12 * s1 * s7  - s12 * s3 * s5;
	inv[11] = -s0 * s5  * s11 + s0 * s7  * s9  + s4 * s1 * s11 - s4 * s3 * s9  - s8  * s1 * s7  + s8  * s3 * s5;
	inv[12] = -s4 * s9  * s14 + s4 * s10 * s13 + s8 * s5 * s14 - s8 * s6 * s13 - s12 * s5 * s10 + s12 * s6 * s9;
	inv[13] =  s0 * s9  * s14 - s0 * s10 * s13 - s8 * s1 * s14 + s8 * s2 * s13 + s12 * s1 * s10 - s12 * s2 * s9;
	inv[14] = -s0 * s5  * s14 + s0 * s6  * s13 + s4 * s1 * s14 - s4 * s2 * s13 - s12 * s1 * s6  + s12 * s2 * s5;
	inv[15] =  s0 * s5  * s10 - s0 * s6  * s9  - s4 * s1 * s10 + s4 * s2 * s9  + s8  * s1 * s6  - s8  * s2 * s5;

	float det = s0 * inv[0] + s1 * inv[4] + s2 * inv[8] + s3 * inv[12];
	if( det != 0.0 )
	{
		det = 1.0 / det;
	}
	for( int i = 0; i < 16; i++ )
	{
		inv[i] = inv[i] * det;
	}

	return *(Mat4*)inv;
}

Mat4 transpose(const Mat4& mat)
{
	Mat4 result;
	for(u32 x=0; x<4; ++x)
		for(u32 y=0; y<4; ++y)
			result[x][y] = mat[y][x];

	return result;
}

void decompose(const Mat4& mat, Vec3* translation, Quat* rotation, Vec3* scale)
{
	*translation = Vec3(mat[3]);
	*scale = Vec3(
		length(mat[0]),
		length(mat[1]),
		length(mat[2])
	);

	Mat4 rotation_matrix;
	rotation_matrix[0] = mat[0] / scale->x;
	rotation_matrix[1] = mat[1] / scale->y;
	rotation_matrix[2] = mat[2] / scale->z;
	*rotation = mat_to_quat(rotation_matrix);
}

Mat4 mat_position(const Vec3& position)
{
	Mat4 mat;
	mat[3] = Vec4(position, 1.f);

	return mat;
}

Mat4 mat_position_rotation(const Vec3& position, const Quat& rotation)
{
	Mat4 mat = quat_to_mat(rotation);
	mat[3] = Vec4(position, 1.f);

	return mat;
}

Mat4 mat_position_scale(const Vec3& position, const Vec3& scale)
{
	return Mat4(
		scale.x, 0.f, 0.f, 0.f,
		0.f, scale.y, 0.f, 0.f,
		0.f, 0.f, scale.z, 0.f,
		position.x, position.y, position.z, 1.f
	);
}

Mat4 mat_position_rotation_scale(const Vec3& position, const Quat& rotation, const Vec3& scale)
{
	Mat4 scale_mat = Mat4(
		scale.x, 0.f, 0.f, 0.f,
		0.f, scale.y, 0.f, 0.f,
		0.f, 0.f, scale.z, 0.f,
		0.f, 0.f, 0.f, 1.f
	);
	Mat4 rotation_mat = quat_to_mat(rotation);
	Mat4 result = rotation_mat * scale_mat;

	result[3] = Vec4(position, 1.f);
	return result;
}