#pragma once
#include "Matrix.h"

struct Transform
{
	Vec3 position = Vec3_Zero;
	Quat rotation = Quat_Identity;
	Vec3 scale = Vec3_One;
};

Transform operator*(const Transform& left, const Transform& right);
Transform inverse(const Transform& val);

Mat4 transform_mat(const Transform& transform);
Transform transform_from_mat(const Mat4& mat);