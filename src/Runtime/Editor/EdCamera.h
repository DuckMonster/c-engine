#pragma once

// Undefine windows shit
#undef near
#undef far

const float ed_cam_fov = radians(60.f);
const float ed_cam_near = 0.5f;
const float ed_cam_far = 100.f;
const float ed_cam_input_scale = 0.005f;
const float ed_cam_move_speed = 2.f;
const float ed_cam_move_speed_fast = 6.f;

struct Ed_Camera
{
	Vec3 position = Vec3(0.f, -5.f, 2.f);
	Vec3 forward = Vec3(0.f, 1.f, -0.2f);

	bool has_control = false;
	Quat orientation = Quat_Identity;
};

void ed_camera_update(Ed_Camera* camera);
Mat4 ed_camera_view_matrix(Ed_Camera* camera);
Mat4 ed_camera_projection_matrix(Ed_Camera* camera);