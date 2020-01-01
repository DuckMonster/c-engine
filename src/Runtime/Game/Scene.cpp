#include "Scene.h"
#include "Core/Input/Input.h"
#include "Core/Context/Context.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"

Scene scene;

void scene_init()
{
	thing_array_init(&scene.projectiles, MAX_PROJECTILES);
	thing_array_init(&scene.units, MAX_UNITS);
	thing_array_init(&scene.drawables, MAX_DRAWABLES);
	thing_array_init(&scene.billboards, MAX_BILLBOARDS);
	thing_array_init(&scene.line_drawers, MAX_LINE_DRAWERS);
	thing_array_init(&scene.health_bars, MAX_HEALTH_BARS);

#if CLIENT
	scene.floor = scene_make_drawable(mesh_load("Mesh/plane.fbx"), material_load("Material/floor.mat"));
#endif
}

void scene_update()
{
	THINGS_FOREACH(&scene.units)
		unit_update(it);

	THINGS_FOREACH(&scene.projectiles)
		projectile_update(it);

	THINGS_FOREACH(&scene.health_bars)
		health_bar_update(it);
}

Unit* scene_make_unit(i32 id, const Vec2& position)
{
	Unit* unit;
	if (id == -1)
		unit = thing_add(&scene.units);
	else
		unit = thing_add_at(&scene.units, id);

	unit_init(unit, id, position);
	return unit;
}

void scene_destroy_unit(Unit* unit)
{
	unit_free(unit);
	thing_remove(&scene.units, unit);
}

u32 scene_get_free_unit_id()
{
	for(u32 i=0; i<scene.units.size; ++i)
	{
		if (!scene.units[i])
			return i;
	}

	error("No free unit id could be found");
	return -1;
}


Projectile* scene_make_projectile(Unit* owner, u32 proj_id, const Vec2& origin, const Vec2& direction)
{
	Projectile* projectile = thing_add(&scene.projectiles);
	projectile_init(projectile, owner, proj_id, origin, direction);

	return projectile;
}

void scene_destroy_projectile(Projectile* projectile)
{
	projectile_free(projectile);
	thing_remove(&scene.projectiles, projectile);
}

#if CLIENT
Drawable* scene_make_drawable(const Mesh* mesh, const Material* material, const Texture* texture)
{
	Drawable* drawable = thing_add(&scene.drawables);
	drawable_init(drawable, mesh, material, texture);
	return drawable;
}

void scene_destroy_drawable(Drawable* drawable)
{
	thing_remove(&scene.drawables, drawable);
}

Billboard* scene_make_billboard(const Sprite_Sheet* sheet)
{
	Billboard* billboard = thing_add(&scene.billboards);
	billboard_init(billboard, sheet);
	return billboard;
}

void scene_destroy_billboard(Billboard* billboard)
{
	thing_remove(&scene.billboards, billboard);
}

Line_Drawer* scene_make_line_drawer(const Vec3& origin)
{
	Line_Drawer* line_drawer = thing_add(&scene.line_drawers);
	line_drawer_init(line_drawer, origin);
	return line_drawer;
}

void scene_destroy_line_drawer(Line_Drawer* line_drawer)
{
	line_drawer_free(line_drawer);
	thing_remove(&scene.line_drawers, line_drawer);
}

Health_Bar* scene_make_health_bar()
{
	Health_Bar* bar = thing_add(&scene.health_bars);
	health_bar_init(bar);
	return bar;
}

void scene_destroy_health_bar(Health_Bar* bar)
{
	health_bar_free(bar);
	thing_remove(&scene.health_bars, bar);
}

void scene_render(const Render_State& state)
{
	/* Drawables */
	THINGS_FOREACH(&scene.drawables)
		drawable_render(it, state);

	THINGS_FOREACH(&scene.billboards)
		billboard_render(it, state);

	if (state.current_pass == PASS_Game)
	{
		THINGS_FOREACH(&scene.line_drawers)
			line_drawer_render(it, state);

		THINGS_FOREACH(&scene.health_bars)
			health_bar_render(it, state);
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