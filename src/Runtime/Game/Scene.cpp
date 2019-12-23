#include "Scene.h"
#include "Core/Input/Input.h"
#include "Core/Context/Context.h"

Scene scene;

void scene_init()
{
	splist_create(&scene.projectiles, MAX_PROJECTILES);

#if CLIENT
	scene.floor = drawable_load("Mesh/plane.fbx", "Material/floor.mat");
#endif
}

#if CLIENT
Ray scene_mouse_ray()
{
	return scene_screen_to_ray(Vec2(input_mouse_x(), input_mouse_y()));
}

Ray scene_screen_to_ray(Vec2 screen)
{
	Mat4 vp = camera_view_projection_matrix(&scene.camera);
	vp = inverse(vp);

	screen /= Vec2(context.width, context.height);
	screen = screen * 2.f - 1.f;
	screen.y = -screen.y;

	Vec4 world_near = vp * Vec4(screen, -1.f, 1.f);
	world_near /= world_near.w;
	Vec4 world_far = vp * Vec4(screen, 1.f, 1.f);
	world_far /= world_far.w;

	Ray result;
	result.origin = (Vec3)world_near;
	result.direction = normalize(Vec3(world_far - world_near));

	return result;
}

Vec2 scene_project_to_screen(const Vec3& position)
{
	Mat4 view_projection = camera_view_projection_matrix(&scene.camera);
	Vec4 ndc_position = view_projection * Vec4(position, 1.f);

	Vec2 screen = Vec2(ndc_position) * Vec2(0.5f, -0.5f) + 0.5f;
	return screen * Vec2(context.width, context.height);
}
#endif