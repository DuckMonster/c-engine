#include "EdCamera.h"
#include "Core/Context/Context.h"
#include "Core/Input/Input.h"

void ed_camera_update(Ed_Camera* camera)
{
	if (!camera->has_control && input_mouse_button_pressed(Mouse_Btn::Right))
	{
		camera->has_control = true;
		context_lock_cursor();
		context_hide_cursor();
	}

	if (camera->has_control && !input_mouse_button_down(Mouse_Btn::Right))
	{
		camera->has_control = false;
		context_unlock_cursor();
		context_show_cursor();
	}

	if (camera->has_control)
	{
		// Rotation
		float delta_x = input_mouse_delta_x();
		float delta_y = input_mouse_delta_y();

		Quat yaw_delta = angle_axis(-delta_x * ed_cam_input_scale, Vec3_Z);
		Quat pitch_delta = angle_axis(delta_y * ed_cam_input_scale, Vec3_Y);

		camera->orientation = yaw_delta * camera->orientation * pitch_delta;

		// Movement
		Vec3 move_direction;
		if (input_key_down(Key::W))
			move_direction += quat_x(camera->orientation);
		if (input_key_down(Key::S))
			move_direction -= quat_x(camera->orientation);

		if (input_key_down(Key::D))
			move_direction -= quat_y(camera->orientation);
		if (input_key_down(Key::A))
			move_direction += quat_y(camera->orientation);

		if (input_key_down(Key::E) || input_key_down(Key::Spacebar))
			move_direction += Vec3_Z;
		if (input_key_down(Key::Q) || input_key_down(Key::LeftControl))
			move_direction -= Vec3_Z;

		float move_speed = input_key_down(Key::LeftShift) ? ed_cam_move_speed_fast : ed_cam_move_speed;
		camera->position += move_direction *  move_speed * time_delta();
	}
}

Mat4 ed_camera_view_matrix(Ed_Camera* camera)
{
	return mat_look_forward(camera->position, quat_x(camera->orientation), Vec3_Z);
}

Mat4 ed_camera_projection_matrix(Ed_Camera* camera)
{
	float aspect = (float)context.width / context.height;
	return mat_perspective(ed_cam_fov, aspect, ed_cam_near, ed_cam_far);
}