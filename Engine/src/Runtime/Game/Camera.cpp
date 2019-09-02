#include "Camera.h"
#include "Core/Input/Input.h"
#include "Core/Context/Context.h"
#include "Runtime/Scene/Scene.h"

void camera_update(Camera* camera)
{
#if CLIENT
	if (input_key_down(Key::E))
		camera->yaw -= 50.f * time_delta();
	if (input_key_down(Key::Q))
		camera->yaw += 50.f * time_delta();

	Vec3 target = scene.player.position;
	Vec3 diff = target - camera->position;
	camera->position += diff * 5.f * time_delta();
#endif
}

Vec3 camera_forward(Camera* camera)
{
	Quat pitch_quat = angle_axis(radians(-camera->pitch), Vec3_X);
	Quat yaw_quat = angle_axis(radians(camera->yaw), Vec3_Z);

	Quat rotation_quat = yaw_quat * pitch_quat;
	return rotation_quat * Vec3_Y;
}

Vec3 camera_right(Camera* camera)
{
	return normalize(cross(camera_forward(camera), Vec3_Y));
}

Mat4 camera_view_matrix(Camera* camera)
{
	Vec3 forward = camera_forward(camera);
	Vec3 eye_position = camera->position - forward * camera->distance;

	return mat_look_forward(eye_position, forward, Vec3_Z);
}

Mat4 camera_projection_matrix(Camera* camera)
{
	float ratio = (float)context.width / (float)context.height;
	return mat_ortho(-5.f * ratio, 5.f * ratio, -5.f, 5.f, -50.f, 50.f);
}

Mat4 camera_view_projection_matrix(Camera* camera)
{
	return camera_projection_matrix(camera) * camera_view_matrix(camera);
}