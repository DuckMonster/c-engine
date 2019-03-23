#include "Camera.h"
#include "Core/Context/Context.h"
#include "Core/Time/Time.h"
#include "Runtime/Scene/Scene.h"

void camera_update(Entity_Camera* camera)
{
	Vec3 offset(0.f, -5.f, 5.f);

	Entity_Player& player = scene.player;
	Vec3 target = player.position + offset;

	camera->position += (target - camera->position) * time_delta();
	camera->direction = -offset;
}

void camera_get_matrix(Entity_Camera* camera, Mat4* out_mat)
{
	float ratio = (float)context.width / (float)context.height;
	Mat4 view, projection;
	mat_ortho(&projection, -5.f * ratio, 5.f * ratio, -5.f, 5.f, -20.f, 20.f);
	mat_look_forward(&view, camera->position, camera->direction, Vec3_Z);

	*out_mat = projection * view;
}