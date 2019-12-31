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
	scene.floor = scene_make_drawable(mesh_load("Mesh/plane.fbx"), material_load("Material/floor.mat"));
#endif
}

void scene_update()
{
	// Update units
	for(u32 i = 0; i<MAX_UNITS; ++i)
	{
		if (scene.unit_enable[i])
			unit_update(scene.units + i);
	}

	// Update projectiles
	for(u32 i=0; i<MAX_PROJECTILES; ++i)
	{
		//if (scene.projectile_enable[i])
			//projectile_update(scene.projectiles + i);
	}
}

Unit* scene_make_unit(i32 id, const Vec2& position)
{
	// If ID isn't specified, find an available one
	if (id == -1)
	{
		for(u32 i=0; i<MAX_UNITS; ++i)
		{
			if (!scene.unit_enable[i])
			{
				id = i;
				break;
			}
		}
	}

	assert_msg(!scene.unit_enable[id], "Tried to spawn unit at already enabled ID");
	scene.unit_enable[id] = true;
	unit_init(scene.units + id, id, position);

	return scene.units + id;
}

void scene_destroy_unit(Unit* unit)
{
	u32 index = unit - scene.units;
	assert_msg(index < MAX_UNITS, "Tried to destroy unit not in scene list");
	assert_msg(scene.unit_enable[index], "Tried to destroy unit that wasn't enabled");

	scene.unit_enable[index] = false;
	unit_free(unit);
}

#if CLIENT
Drawable* scene_make_drawable(const Mesh* mesh, const Material* material, const Texture* texture)
{
	for(u32 i=0; i<MAX_DRAWABLES; ++i)
	{
		if (!scene.drawable_enable[i])
		{
			scene.drawable_enable[i] = true;
			drawable_init(scene.drawables + i, mesh, material, texture);

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
	assert_msg(scene.drawable_enable[index], "Tried to destroy drawable that wasn't enabled");

	scene.drawable_enable[index] = false;
	*drawable = Drawable();
}

Billboard* scene_make_billboard(const Sprite_Sheet* sheet)
{
	for(u32 i=0; i<MAX_BILLBOARDS; ++i)
	{
		if (!scene.billboard_enable[i])
		{
			scene.billboard_enable[i] = true;
			billboard_init(scene.billboards + i, sheet);

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
	assert_msg(scene.billboard_enable[index], "Tried to destroy billboard that wasn't enabled");

	scene.billboard_enable[index] = false;

	*billboard = Billboard();
}

Line_Drawer* scene_make_line_drawer(const Vec3& origin)
{
	for(u32 i=0; i<MAX_LINE_DRAWERS; ++i)
	{
		if (!scene.line_drawer_enable[i])
		{
			scene.line_drawer_enable[i] = true;
			line_drawer_init(scene.line_drawers + i, origin);

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
	assert_msg(scene.line_drawer_enable[index], "Tried to destroy line drawer that wasn't enabled");

	scene.line_drawer_enable[index] = false;

	line_drawer_free(line_drawer);
	*line_drawer = Line_Drawer();
}

Health_Bar* scene_make_health_bar()
{
	for(u32 i=0; i<MAX_LINE_DRAWERS; ++i)
	{
		if (!scene.health_bar_enable[i])
		{
			scene.health_bar_enable[i] = true;
			health_bar_init(scene.health_bars + i);

			return scene.health_bars + i;
		}
	}

	error("Ran out of health bars in scene");
	return nullptr;
}

void scene_destroy_health_bar(Health_Bar* bar)
{
	u32 index = bar - scene.health_bars;
	assert_msg(index < MAX_HEALTH_BARS, "Tried to destroy health bar that was not in scene list");
	assert_msg(scene.health_bar_enable[index], "Tried to destroy health bar that wasn't enabled");

	scene.health_bar_enable[index] = false;

	health_bar_free(bar);
	*bar = Health_Bar();
}

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

	/* Health bars */
	for(u32 i=0; i<MAX_HEALTH_BARS; ++i)
	{
		if (!scene.health_bar_enable[i])
			continue;

		health_bar_render(scene.health_bars + i, state);
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