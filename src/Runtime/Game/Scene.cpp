#include "Scene.h"
#include "Core/Input/Input.h"
#include "Core/Context/Context.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"

Scene scene;

void scene_init()
{
	splist_create(&scene.projectiles, MAX_PROJECTILES);

#if CLIENT
	scene.floor = scene_make_drawable();
	drawable_init(scene.floor, mesh_load("Mesh/plane.fbx"), material_load("Material/floor.mat"));
#endif
}

Drawable* scene_make_drawable()
{
	for(u32 i=0; i<MAX_DRAWABLES; ++i)
	{
		if (!scene.drawable_enable[i])
		{
			scene.drawable_enable[i] = true;
			return scene.drawables + i;
		}
	}

	error("Ran out of drawables in scene");
	return nullptr;
}

void scene_destroy_drawable(Drawable* drawable)
{
	u32 index = drawable - scene.drawables;
	assert_msg(index < MAX_DRAWABLES, "Tried to destroy drawable that was not in scene list");
	assert_msg(scene.drawables + index == drawable, "Tried to destroy drawable that was not in scene list");
	assert_msg(scene.drawable_enable[index], "Tried to destroy drawable that wasn't enabled");

	scene.drawable_enable[index] = false;
	*drawable = Drawable();
}

Billboard* scene_make_billboard()
{
	for(u32 i=0; i<MAX_BILLBOARDS; ++i)
	{
		if (!scene.billboard_enable[i])
		{
			scene.billboard_enable[i] = true;
			return scene.billboards + i;
		}
	}

	error("Ran out of billboards in scene");
	return nullptr;
}

void scene_destroy_billboard(Billboard* billboard)
{
	u32 index = billboard - scene.billboards;
	assert_msg(index < MAX_BILLBOARDS, "Tried to destroy billboard that was not in scene list");
	assert_msg(scene.billboards + index == billboard, "Tried to destroy billboard that was not in scene list");
	assert_msg(scene.billboard_enable[index], "Tried to destroy billboard that wasn't enabled");

	scene.billboard_enable[index] = false;
	*billboard = Billboard();
}

Line_Drawer* scene_make_line_drawer()
{
	for(u32 i=0; i<MAX_LINE_DRAWERS; ++i)
	{
		if (!scene.line_drawer_enable[i])
		{
			scene.line_drawer_enable[i] = true;
			return scene.line_drawers + i;
		}
	}

	error("Ran out of line drawers in scene");
	return nullptr;
}

void scene_destroy_line_drawer(Line_Drawer* line_drawer)
{
	u32 index = line_drawer - scene.line_drawers;
	assert_msg(index < MAX_LINE_DRAWERS, "Tried to destroy line drawer that was not in scene list");
	assert_msg(scene.line_drawers + index == line_drawer, "Tried to destroy line drawer that was not in scene list");
	assert_msg(scene.line_drawer_enable[index], "Tried to destroy line drawer that wasn't enabled");

	scene.line_drawer_enable[index] = false;
	*line_drawer = Line_Drawer();
}

#if CLIENT
void scene_render(const Render_State& state)
{
	/* Drawables */
	for(u32 i=0; i<MAX_DRAWABLES; ++i)
	{
		if (!scene.drawable_enable[i])
			continue;

		drawable_render(scene.drawables + i, state);
	}

	/* Billboards */
	for(u32 i=0; i<MAX_BILLBOARDS; ++i)
	{
		if (!scene.billboard_enable[i])
			continue;

		billboard_render(scene.billboards + i, state);
	}

	/* Line drawers */
	for(u32 i=0; i<MAX_LINE_DRAWERS; ++i)
	{
		if (!scene.line_drawer_enable[i])
			continue;

		line_drawer_render(scene.line_drawers + i, state);
	}
}

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