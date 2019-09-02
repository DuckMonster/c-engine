#pragma once

struct Camera
{
	Vec3 position;

	float yaw = 0.f;
	float pitch = 45.f;
	float distance = 5.f;
};

void camera_update(Camera* camera);
Vec3 camera_forward(Camera* camera);
Vec3 camera_right(Camera* camera);
Mat4 camera_view_matrix(Camera* camera);
Mat4 camera_projection_matrix(Camera* camera);
Mat4 camera_view_projection_matrix(Camera* camera);