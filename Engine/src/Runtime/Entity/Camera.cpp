#include "Camera.h"
#include "Core/Context/Context.h"
#include "Core/Time/Time.h"
#include "Runtime/Scene/Scene.h"
#include "Core/Input/Input.h"

void camera_update(Entity_Camera* camera)
{
	if (input_key_down(Key::E))
		camera->offset_angle += 0.7f * time_delta();
	if (input_key_down(Key::Q))
		camera->offset_angle -= 0.7f * time_delta();

	Vec3 offset = Vec3(-cos(camera->offset_angle), sin(camera->offset_angle), 1.f) * 5.f;

	Entity_Player& player = scene.player;
	Vec3 target = player.position + offset;

	camera->position += (target - camera->position) * 4.f * time_delta();
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

Ray get_mouse_ray(Entity_Camera* camera)
{
	Vec2 mouse(input_mouse_x(), input_mouse_y());
	debug_log("mouse= { %f, %f }", mouse.x, mouse.y);
	mouse.x /= context.width;
	mouse.y /= context.height;

	mouse = mouse * 2.f - 1.f;
	mouse.y = -mouse.y;


	Vec4 near(mouse, 1.f, 1.f);
	Vec4 far(mouse, -1.f, 1.f);

	// Deproject
	Mat4 inv_vp;
	camera_get_matrix(camera, &inv_vp);
	inv_vp = inverse(inv_vp);

	near = inv_vp * near;
	near /= near.w;
	far = inv_vp * far;
	far /= far.w;

	debug_log("near = { %f, %f, %f }", near.x, near.y, near.z);
	debug_log("far  = { %f, %f, %f }", far.x, far.y, far.z);

	return ray_from_to(Vec3(near), Vec3(far));
}