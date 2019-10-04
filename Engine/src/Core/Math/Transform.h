#pragma once
#include "Matrix.h"

struct Transform
{
	Vec3 position = Vec3_Zero;
	Quat rotation = Quat_Identity;
	Vec3 scale = Vec3_One;
};

Mat4 transform_mat(const Transform& transform);