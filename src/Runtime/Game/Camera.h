#pragma once
#if CLIENT

const float camera_offset_impulse = 3.f;
const float camera_offset_accleration = 690.f;
const float camera_offset_drag = 14.f;

const float camera_angular_impulse = 0.2f;
const float camera_angular_acceleration = 200.f;
const float camera_angular_drag = 12.f;

struct Camera
{
	Vec3 target_position;
	Vec3 position;

	Vec3 offset;
	Vec3 offset_velocity;
	float angle;
	float angle_velocity;

	float yaw = 0.f;
	float pitch = 40.f;
	float distance = 5.f;
	float size = 10.f;
	float zoom = 1.f;
};

void camera_update(Camera* camera);
Vec3 camera_forward(Camera* camera);
Vec3 camera_right(Camera* camera);
Mat4 camera_view_matrix(Camera* camera);
Mat4 camera_projection_matrix(Camera* camera);
Mat4 camera_view_projection_matrix(Camera* camera);

void camera_add_impulse(Vec3 direction, float force);

#endif