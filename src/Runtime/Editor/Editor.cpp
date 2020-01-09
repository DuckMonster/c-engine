#include "Editor.h"
#include "Core/Input/Input.h"
#include "Runtime/Render/Render.h"
#include "Runtime/Game/Scene.h"
#include "Engine/Collision/CollisionTypes.h"
#include "Engine/Collision/HitTest.h"

#if CLIENT
Ray ray;
Aligned_Box aligned_box;
Box box;

static void draw_hit_result(const Hit_Result& hit)
{
	if (hit.has_hit)
	{
		scene_draw_point(hit.position, Color_Red);
		scene_draw_line(hit.position, hit.position + hit.normal, Color_Red);
	}
}

void editor_init(Editor* editor)
{
	aligned_box.position = Vec3(-5.f, 2.f, 2.f);
	aligned_box.size = Vec3(3.f, 0.5f, 1.f);

	box.position = Vec3(-6.f, -4.f, 5.f);
	box.size = Vec3(1.f, 5.f, 1.2f);
	box.orientation = angle_axis(2.f, normalize(Vec3(2.f, 1.f, -6.f)));
}

void editor_update(Editor* editor)
{
	ed_camera_update(&editor->camera);
	render_set_vp(ed_camera_view_matrix(&editor->camera), ed_camera_projection_matrix(&editor->camera));

	/*** Test collision stuff ***/
	if (input_key_down(Key::R))
	{
		ray.origin = editor->camera.position - quat_y(editor->camera.orientation) * 0.5f;
		ray.direction = quat_x(editor->camera.orientation);
	}

	// Convert ray to line
	Line_Trace line;
	line.start = ray.origin;
	line.end = ray.origin + ray.direction * 5.f;

	scene_draw_line(line.start, line.end, Color_Green);

	// Ray-plane intersection
	Plane plane;
	plane.point = Vec3_Zero;
	plane.normal = Vec3_Z;

	Hit_Result hit = test_line_trace_plane(line, plane);
	draw_hit_result(hit);

	// Ray-sphere intersection
	Sphere sphere;
	sphere.origin = Vec3_Z * 1.2f;
	sphere.radius = 2.f;

	scene_draw_sphere(sphere.origin, sphere.radius, Color_Blue);
	hit = test_line_trace_sphere(line, sphere);
	draw_hit_result(hit);

	// Ray-aligned_box intersection
	scene_draw_box(aligned_box.position, aligned_box.size, Quat_Identity, Color_Blue);

	hit = test_line_trace_aligned_box(line, aligned_box);
	draw_hit_result(hit);

	// Ray-box intersection
	scene_draw_box(box.position, box.size, box.orientation, Color_Blue);
	hit = test_line_trace_box(line, box);
	draw_hit_result(hit);
}

void editor_render(Editor* editor)
{

}

#endif