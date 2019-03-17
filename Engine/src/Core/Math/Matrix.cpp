#include "Matrix.h"

void ortho(Mat4* out_mat, float left, float right, float bottom, float top, float near, float far)
{
	*out_mat = Mat4(
		2.f / (right - left),				0.f,								0.f,							0.f,
		0.f,								2.f / (top - bottom),				0.f,							0.f,
		0.f,								0.f,								2.f / (far - near),				0.f,
		-((right + left) / (right - left)),	-((top + bottom) / (top - bottom)), -((far + near) / (far - near)), 1.f
	);
}

void look_at(Mat4* out_mat, const Vec3& eye, const Vec3& target, const Vec3& up)
{

}