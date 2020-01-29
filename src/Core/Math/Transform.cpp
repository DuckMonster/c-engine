#include "Transform.h"

Mat4 transform_mat(const Transform& transform)
{
	return mat_position_rotation_scale(transform.position, transform.rotation, transform.scale);
}

Transform transform_from_mat(const Mat4& mat)
{
	Transform result;
	decompose(mat, &result.position, &result.rotation, &result.scale);

	return result;
}