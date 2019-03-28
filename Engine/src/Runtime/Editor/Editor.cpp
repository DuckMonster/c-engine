#include "Editor.h"
#include "Core/Context/Context.h"
#include "Core/Input/Input.h"
#include "Engine/Graphics/Render.h"
#include "Runtime/Scene/Scene.h"
#include "Runtime/Entity/Entity.h"
#include "Gizmo.h"

Editor editor;

namespace
{
	Material_Standard floor_mat;
}

void editor_init()
{
	material_load_standard(&floor_mat, "res/floor.vert", "res/floor.frag");
	gizmo_init();
}

void editor_begin()
{
	editor.camera_position = scene.camera.position - quat_x(scene.camera.rotation) * 15.f;
	editor.camera_rotation = scene.camera.rotation;

	editor.selected_entity = (Entity*)&scene.player;
}

void editor_end()
{
}

void editor_update_and_render()
{
	// Pilot camera? 
	bool should_pilot = input_mouse_button_down(Mouse_Btn::Right);
	if (editor.piloting_camera != should_pilot)
	{
		editor.piloting_camera = should_pilot;
		if (editor.piloting_camera)
		{
			context_hide_cursor();
			context_lock_cursor();
		}
		else
		{
			context_show_cursor();
			context_unlock_cursor();
		}
	}

	// Move camera
	if (editor.piloting_camera)
	{
		Vec3 camera_right = -quat_y(editor.camera_rotation);
		Vec3 camera_forward = quat_x(editor.camera_rotation);
		Vec3 camera_up = quat_z(editor.camera_rotation);

		Vec3 move_direction;
		if (input_key_down(Key::D))
			move_direction += camera_right;
		if (input_key_down(Key::A))
			move_direction -= camera_right;
		if (input_key_down(Key::W))
			move_direction += camera_forward;
		if (input_key_down(Key::S))
			move_direction -= camera_forward;
		if (input_key_down(Key::E))
			move_direction += camera_up;
		if (input_key_down(Key::Q))
			move_direction -= camera_up;

		editor.camera_position += move_direction * 10.f * time_delta();

		// Look around
		float yaw_delta = (float)input_mouse_delta_x() * 0.005;
		float pitch_delta = (float)input_mouse_delta_y() * 0.005;

		editor.camera_rotation = angle_axis(-yaw_delta, Vec3_Z) * editor.camera_rotation;
		editor.camera_rotation = editor.camera_rotation * angle_axis(pitch_delta, Vec3_Y);
	}

	// Set camera VP for rendering
	float aspect = (float)context.width / context.height;

	Mat4 perspective, view;
	mat_perspective(&perspective, 0.7f, aspect, 1.f, 100.f);
	mat_look_forward(&view, editor.camera_position, quat_x(editor.camera_rotation), Vec3_Z);

	render_set_vp(perspective * view);


	player_render(&scene.player);
	enemies_render();

	float floor_verts[] = {
		-1.f, -1.f, 0.f,
		1.f, -1.f, 0.f,
		1.f, 1.f, 0.f,

		-1.f, -1.f, 0.f,
		1.f, 1.f, 0.f,
		-1.f, 1.f, 0.f
	};

	// Render floor
	render_begin_material_standard(floor_mat);
	render_draw_immediate(6, floor_verts, nullptr, nullptr);

	gizmo_update_and_render();
}