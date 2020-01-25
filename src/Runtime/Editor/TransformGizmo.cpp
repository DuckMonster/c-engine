#include "TransformGizmo.h"
#include "Core/Input/Input.h"
#include "Engine/Collision/HitTest.h"
#include "Runtime/Game/Scene.h"
#include "Runtime/Game/Game.h"

#if CLIENT
Vec3 get_best_drag_plane_normal(const Vec3& a, const Vec3& b)
{
	Vec3 camera_forw = quat_x(game.editor.camera.orientation);
	float a_dot = abs(dot(camera_forw, a));
	float b_dot = abs(dot(camera_forw, b));

	debug_log("%f > %f", a_dot, b_dot);

	return a_dot > b_dot ? a : b;
}

void gizmo_enable_axis(Transform_Gizmo* gizmo, Gizmo_Axis axis)
{
	gizmo->active_axes |= axis;
}
void gizmo_disable_axis(Transform_Gizmo* gizmo, Gizmo_Axis axis)
{
	gizmo->active_axes &= ~axis;
}

Plane gizmo_get_drag_plane(Transform_Gizmo* gizmo)
{
	int axes = gizmo->active_axes;
	Vec3 gizmo_x = quat_x(gizmo->rotation);
	Vec3 gizmo_y = quat_y(gizmo->rotation);
	Vec3 gizmo_z = quat_z(gizmo->rotation);

	if (axes == Axis_X)
		return plane_make(gizmo->position, get_best_drag_plane_normal(gizmo_y, gizmo_z));
	if (axes == Axis_Y)
		return plane_make(gizmo->position, get_best_drag_plane_normal(gizmo_x, gizmo_z));
	if (axes == Axis_Z)
		return plane_make(gizmo->position, get_best_drag_plane_normal(gizmo_x, gizmo_y));

	if (axes == (Axis_X | Axis_Y))
		return plane_make(gizmo->position, gizmo_z);
	if (axes == (Axis_X | Axis_Z))
		return plane_make(gizmo->position, gizmo_y);
	if (axes == (Axis_Y | Axis_Z))
		return plane_make(gizmo->position, gizmo_x);

	return plane_make(gizmo->position, -quat_x(game.editor.camera.orientation));
}

void gizmo_init(Transform_Gizmo* gizmo)
{
	gizmo->rotation = angle_axis(2.f, normalize(Vec3(10.f, -2.f, -8.f)));
}

void gizmo_update(Transform_Gizmo* gizmo)
{
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

	// Get mouse delta
	Plane drag_plane = gizmo_get_drag_plane(gizmo);
	scene_draw_line(drag_plane.point, drag_plane.point + drag_plane.normal);

	Ray mouse_ray = editor_mouse_ray();

	Hit_Result prev_hit = test_ray_plane(gizmo->prev_mouse_ray, drag_plane);
	Hit_Result new_hit = test_ray_plane(mouse_ray, drag_plane);

	Vec3 delta = new_hit.position - prev_hit.position;
	gizmo->prev_mouse_ray = mouse_ray;

	debug_log("(%f, %f, %f)", prev_hit.position.x, prev_hit.position.y, prev_hit.position.z);

	Vec3 actual_delta = Vec3_Zero;
	if (gizmo->active_axes & Axis_X)
		actual_delta += constrain_to_direction(delta, quat_x(gizmo->rotation));
	if (gizmo->active_axes & Axis_Y)
		actual_delta += constrain_to_direction(delta, quat_y(gizmo->rotation));
	if (gizmo->active_axes & Axis_Z)
		actual_delta += constrain_to_direction(delta, quat_z(gizmo->rotation));

	gizmo->position += actual_delta;
}

void gizmo_draw(Transform_Gizmo* gizmo)
{
	Vec3 forw = quat_x(gizmo->rotation);
	Vec3 right = quat_y(gizmo->rotation);
	Vec3 up = quat_z(gizmo->rotation);
	scene_draw_point(gizmo->position);

	if (gizmo->active_axes == 0)
	{
		scene_draw_box(gizmo->position + forw * 0.25f, Vec3(0.3f, 0.05f, 0.05f), gizmo->rotation, Color_Red);
		scene_draw_box(gizmo->position + right * 0.25f, Vec3(0.05f, 0.3f, 0.05f), gizmo->rotation, Color_Green);
		scene_draw_box(gizmo->position + up * 0.25f, Vec3(0.05f, 0.05f, 0.3f), gizmo->rotation, Color_Blue);
	}
	else
	{
		if (gizmo->active_axes & Axis_X)
			scene_draw_box(gizmo->position + forw * 0.25f, Vec3(0.3f, 0.05f, 0.05f), gizmo->rotation, Color_Red);
		if (gizmo->active_axes & Axis_Y)
			scene_draw_box(gizmo->position + right * 0.25f, Vec3(0.05f, 0.3f, 0.05f), gizmo->rotation, Color_Green);
		if (gizmo->active_axes & Axis_Z)
			scene_draw_box(gizmo->position + up * 0.25f, Vec3(0.05f, 0.05f, 0.3f), gizmo->rotation, Color_Blue);
	}

}

Mat4 gizmo_get_transform(Transform_Gizmo* gizmo)
{
	return mat_position_rotation_scale(gizmo->position, gizmo->rotation, gizmo->scale);
}

void gizmo_set_transform(Transform_Gizmo* gizmo, const Mat4& transform)
{

}
#endif