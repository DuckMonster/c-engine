#include "TransformGizmo.h"
#include "Core/Input/Input.h"
#include "Core/Context/Context.h"
#include "Engine/Collision/HitTest.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include "Runtime/Game/Scene.h"
#include "Runtime/Game/Game.h"

#if CLIENT
Quat gizmo_space_quat(Transform_Gizmo* gizmo)
{
	if (gizmo->space == Space_Local)
		return gizmo->transform.rotation;
	else
		return gizmo->world_temp_rotation;
}
Vec3 gizmo_x(Transform_Gizmo* gizmo)
{
	return quat_x(gizmo_space_quat(gizmo));
}
Vec3 gizmo_y(Transform_Gizmo* gizmo)
{
	return quat_y(gizmo_space_quat(gizmo));
}
Vec3 gizmo_z(Transform_Gizmo* gizmo)
{
	return quat_z(gizmo_space_quat(gizmo));
}

Vec3 get_best_drag_plane_normal(Transform_Gizmo* gizmo, const Vec3& a, const Vec3& b)
{
	Vec3 forw = gizmo->transform.position - game.editor.camera.position;
	float a_dot = abs(dot(forw, a));
	float b_dot = abs(dot(forw, b));

	return a_dot > b_dot ? a : b;
}

Plane gizmo_calculate_drag_plane(Transform_Gizmo* gizmo)
{
	int axes = gizmo->active_axes;

	if (gizmo->mode == Mode_Translate)
	{
		if (axes == Axis_X)
			return plane_make(gizmo->transform.position, get_best_drag_plane_normal(gizmo, gizmo_y(gizmo), gizmo_z(gizmo)));
		if (axes == Axis_Y)
			return plane_make(gizmo->transform.position, get_best_drag_plane_normal(gizmo, gizmo_x(gizmo), gizmo_z(gizmo)));
		if (axes == Axis_Z)
			return plane_make(gizmo->transform.position, get_best_drag_plane_normal(gizmo, gizmo_x(gizmo), gizmo_y(gizmo)));

		if (axes == (Axis_X | Axis_Y))
			return plane_make(gizmo->transform.position, gizmo_z(gizmo));
		if (axes == (Axis_X | Axis_Z))
			return plane_make(gizmo->transform.position, gizmo_y(gizmo));
		if (axes == (Axis_Y | Axis_Z))
			return plane_make(gizmo->transform.position, gizmo_x(gizmo));
	}
	else if (gizmo->mode == Mode_Rotate)
	{
		if (axes & Axis_X)
			return plane_make(gizmo->transform.position, gizmo_x(gizmo));
		if (axes & Axis_Y)
			return plane_make(gizmo->transform.position, gizmo_y(gizmo));
		if (axes & Axis_Z)
			return plane_make(gizmo->transform.position, gizmo_z(gizmo));
	}

	return plane_make(gizmo->transform.position, -quat_x(game.editor.camera.orientation));
}

void gizmo_disable_axis(Transform_Gizmo* gizmo, Gizmo_Axis axis)
{
	gizmo->active_axes &= ~axis;

	gizmo->drag_plane = gizmo_calculate_drag_plane(gizmo);
	gizmo->world_temp_rotation = Quat_Identity;
}
void gizmo_enable_axis(Transform_Gizmo* gizmo, Gizmo_Axis axis)
{
	// If we rotate, we only want to select one axis at a time
	if (gizmo->mode == Mode_Rotate)
	 	gizmo_disable_axis(gizmo, Axis_All);

	gizmo->active_axes |= axis;
	gizmo->prev_mouse_ray = editor_mouse_ray();

	gizmo->drag_plane = gizmo_calculate_drag_plane(gizmo);
	gizmo->world_temp_rotation = Quat_Identity;
}

void gizmo_set_mode(Transform_Gizmo* gizmo, Gizmo_Mode mode)
{
	gizmo_disable_axis(gizmo, Axis_All);
	gizmo->mode = mode;

	gizmo->drag_plane = gizmo_calculate_drag_plane(gizmo);
	gizmo->world_temp_rotation = Quat_Identity;
}

void gizmo_init(Transform_Gizmo* gizmo)
{
	gizmo->material = material_load("Material/Gizmo/gizmo.mat");
	gizmo->meshes[Mode_Translate] = mesh_load("Mesh/Gizmo/translate.fbx");
	gizmo->meshes[Mode_Rotate] = mesh_load("Mesh/Gizmo/rotate.fbx");
	gizmo->meshes[Mode_Scale] = mesh_load("Mesh/Gizmo/scale.fbx");

	framebuffer_create(&gizmo->draw_buffer, context.width, context.height);
	framebuffer_add_color_texture(&gizmo->draw_buffer);
	framebuffer_add_depth_texture(&gizmo->draw_buffer);
}

void gizmo_apply_translate(Transform_Gizmo* gizmo, const Vec3& from, const Vec3& to)
{
	Vec3 delta = to - from;
	Vec3 translate_delta = Vec3_Zero;
	if (gizmo->active_axes & Axis_X)
		translate_delta += constrain_to_direction(delta, gizmo_x(gizmo));
	if (gizmo->active_axes & Axis_Y)
		translate_delta += constrain_to_direction(delta, gizmo_y(gizmo));
	if (gizmo->active_axes & Axis_Z)
		translate_delta += constrain_to_direction(delta, gizmo_z(gizmo));

	gizmo->transform.position += translate_delta;
}

void gizmo_apply_rotate(Transform_Gizmo* gizmo, const Vec3& from, const Vec3& to)
{
	Vec3 from_offset = from - gizmo->transform.position;
	Vec3 to_offset = to - gizmo->transform.position;

	Quat delta_quat = quat_from_to(from_offset, to_offset);
	if (is_nan(delta_quat))
	{
		delta_quat = quat_from_to(from_offset, to_offset);
		delta_quat = Quat_Identity;
	}

	gizmo->transform.rotation = delta_quat * gizmo->transform.rotation;
	gizmo->world_temp_rotation = delta_quat * gizmo->world_temp_rotation;
}

void gizmo_apply_scale(Transform_Gizmo* gizmo, const Vec3& from, const Vec3& to)
{
	float dist_from = distance(from, gizmo->transform.position);
	float dist_to = distance(to, gizmo->transform.position);

	float scale_delta = dist_to / dist_from;

	Vec3 new_scale = gizmo->transform.scale;
	if (gizmo->active_axes & Axis_X)
		new_scale.x *= scale_delta;
	if (gizmo->active_axes & Axis_Y)
		new_scale.y *= scale_delta;
	if (gizmo->active_axes & Axis_Z)
		new_scale.z *= scale_delta;

	gizmo->transform.scale = new_scale;
}

void gizmo_update(Transform_Gizmo* gizmo)
{
	// Toggle space
	if (input_key_pressed(Key::X))
		gizmo->space = (gizmo->space + 1) % Space_Max;

	// Set modes
	if (input_key_pressed(Key::Q))
		gizmo_set_mode(gizmo, Mode_None);
	if (input_key_pressed(Key::W))
		gizmo_set_mode(gizmo, Mode_Translate);
	if (input_key_pressed(Key::E))
		gizmo_set_mode(gizmo, Mode_Rotate);
	if (input_key_pressed(Key::R))
		gizmo_set_mode(gizmo, Mode_Scale);

	// X axis
	if (input_key_pressed(Key::A))
		gizmo_enable_axis(gizmo, Axis_X);
	if (input_key_released(Key::A))
		gizmo_disable_axis(gizmo, Axis_X);

	if (input_key_pressed(Key::S))
		gizmo_enable_axis(gizmo, Axis_Y);
	if (input_key_released(Key::S))
		gizmo_disable_axis(gizmo, Axis_Y);

	if (input_key_pressed(Key::D))
		gizmo_enable_axis(gizmo, Axis_Z);
	if (input_key_released(Key::D))
		gizmo_disable_axis(gizmo, Axis_Z);

	if (gizmo->active_axes != 0)
	{
		// Get mouse delta
		Ray mouse_ray = editor_mouse_ray();

		Hit_Result prev_hit = test_ray_plane(gizmo->prev_mouse_ray, gizmo->drag_plane);
		Hit_Result new_hit = test_ray_plane(mouse_ray, gizmo->drag_plane);

		if (prev_hit.has_hit && new_hit.has_hit)
		{
			Vec3 delta = new_hit.position - prev_hit.position;

			switch(gizmo->mode)
			{
				case Mode_Translate:
					gizmo_apply_translate(gizmo, prev_hit.position, new_hit.position);
					break;

				case Mode_Rotate:
					gizmo_apply_rotate(gizmo, prev_hit.position, new_hit.position);
					break;

				case Mode_Scale:
					gizmo_apply_scale(gizmo, prev_hit.position, new_hit.position);
					break;
			}
		}

		gizmo->prev_mouse_ray = mouse_ray;
	}
}

void gizmo_draw_axis(Transform_Gizmo* gizmo, const Quat& orientation, const Vec4& color, bool active)
{
	if (gizmo->meshes[gizmo->mode] != nullptr)
	{
		material_set(gizmo->material, "u_Model", mat_position_rotation(gizmo->transform.position, orientation));
		material_set(gizmo->material, "u_Color", color);
		mesh_draw(gizmo->meshes[gizmo->mode]);

		if (active)
			scene_draw_line(gizmo->transform.position + quat_x(orientation) * 100.f, gizmo->transform.position - quat_x(orientation) * 100.f, color);
	}
}

void gizmo_draw_axis_x(Transform_Gizmo* gizmo)
{
	gizmo_draw_axis(gizmo, gizmo_space_quat(gizmo), Color_Red, gizmo->active_axes & Axis_X);
}
void gizmo_draw_axis_y(Transform_Gizmo* gizmo)
{
	gizmo_draw_axis(gizmo, quat_from_xz(gizmo_y(gizmo), gizmo_x(gizmo)), Color_Green, gizmo->active_axes & Axis_Y);
}
void gizmo_draw_axis_z(Transform_Gizmo* gizmo)
{
	gizmo_draw_axis(gizmo, quat_from_xz(gizmo_z(gizmo), gizmo_y(gizmo)), Color_Blue, gizmo->active_axes & Axis_Z);
}

void gizmo_draw(Transform_Gizmo* gizmo, const Render_State& state)
{
	Frame_Buffer* current_fb = framebuffer_get_current();

	// We draw the gizmo on a separaret framebuffer so we can do our own
	//	depth testing while ignoring the depth-testing of the main color-pass
	framebuffer_push(&gizmo->draw_buffer);

	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	material_bind(gizmo->material);
	material_set(gizmo->material, "u_ViewProjection", state.view_projection);
	material_set(gizmo->material, "u_ContextSize", Vec2(context.width, context.height));

	// We use the depth-buffer of the main color-pass do make the gizmo
	//	darker if "blocked" by something
	texture_bind(&current_fb->textures[1], 0);

	scene_draw_point(gizmo->transform.position);

	if (gizmo->active_axes == 0)
	{
		gizmo_draw_axis_x(gizmo);
		gizmo_draw_axis_y(gizmo);
		gizmo_draw_axis_z(gizmo);
	}
	else
	{
		if (gizmo->active_axes & Axis_X)
			gizmo_draw_axis_x(gizmo);
		if (gizmo->active_axes & Axis_Y)
			gizmo_draw_axis_y(gizmo);
		if (gizmo->active_axes & Axis_Z)
			gizmo_draw_axis_z(gizmo);
	}

	framebuffer_pop();

	// Render our gizmo-frame-buffer onto the screen
	texture_draw_fullscreen(&gizmo->draw_buffer.textures[0]);

	// Draw a nice rotation line
	if (gizmo->mode == Mode_Rotate && gizmo->active_axes != 0)
	{
		Ray mouse_ray = editor_mouse_ray();
		Hit_Result hit = test_ray_plane(mouse_ray, gizmo->drag_plane);

		if (hit.has_hit)
			scene_draw_line(gizmo->transform.position, hit.position);
	}
}
#endif