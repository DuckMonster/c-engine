#pragma once
#include "Entity.h"

struct Entity_Camera
{
	// Entity type
	const Entity_Type type = Entity_Type::Camera;

	Vec3 position;
	Vec3 direction;
};

void camera_update(Entity_Camera* camera);
void camera_get_matrix(Entity_Camera* camera, Mat4* out_mat);