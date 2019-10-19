#include "Transform.h"

Mat4 transform_mat(const Transform& transform)
{
	return mat_position_rotation_scale(transform.position, transform.rotation, transform.scale);
}