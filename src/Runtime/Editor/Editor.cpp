#include "Editor.h"
#include "Core/Input/Input.h"
#include "Runtime/Render/Render.h"
#include "Runtime/Game/Scene.h"
#include "Engine/Collision/CollisionTypes.h"
#include "Engine/Collision/HitTest.h"

#if CLIENT
Ray ray;

void editor_init(Editor* editor)
{

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
	Line line;
	line.start = ray.origin;
	line.end = ray.origin + ray.direction * 5.f;

	// Ray-plane intersection
	Plane plane;
	plane.point = Vec3_Zero;
	plane.normal = Vec3_Z;

	Hit_Result hit = test_line_plane(line, plane);

	scene_draw_line(line.start, line.end, Color_Green);
	if (hit.has_hit)
	{
		scene_draw_point(hit.position, Color_Red);
		scene_draw_line(hit.position, hit.position + hit.normal, Color_Red);
	}

	// Ray-sphere intersection
	Sphere sphere;
	sphere.origin = Vec3_Z * 1.2f;
	sphere.radius = 2.f;

	scene_draw_sphere(sphere.origin, sphere.radius, Color_Blue);
	hit = test_line_sphere(line, sphere);
	if (hit.has_hit)
	{
		scene_draw_point(hit.position, Color_Red);
		scene_draw_line(hit.position, hit.position + hit.normal, Color_Red);
	}

	// Ray-alignbox intersection
	scene_draw_aligned_box(Vec3(-5.f, 2.f, 2.f), Vec3(3.f, 0.5f, 1.f));
}

void editor_render(Editor* editor)
{

}

#endif