#include "Transform.h"

Transform operator*(const Transform& left, const Transform& right)
{
	// Optimize...
	return transform_from_mat(transform_mat(left) * transform_mat(right));
}

Transform inverse(const Transform& val)
{
	// Optimize...
	return transform_from_mat(inverse(transform_mat(val)));
}

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