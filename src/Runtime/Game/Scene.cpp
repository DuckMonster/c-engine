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
#if CLIENT
	thing_array_init(&scene.drawables, MAX_DRAWABLES);
	thing_array_init(&scene.billboards, MAX_BILLBOARDS);
	thing_array_init(&scene.line_drawers, MAX_LINE_DRAWERS);
	thing_array_init(&scene.health_bars, MAX_HEALTH_BARS);

	primitives_init(&scene.primitive_manager);
#endif

	scene.sphere.origin = Vec3(5.f, 0.f, 0.f);
	scene.sphere.radius = 2.f;

	scene.aligned_box.position = Vec3(-10.f, 3.f, 0.f);
	scene.aligned_box.size = Vec3(4.f, 1.f, 2.f);

	scene.box.position = Vec3(-8.f, -5.f, 0.f);
	scene.box.size = Vec3(4.f, 2.f, 6.f);
	scene.box.orientation = angle_axis(1.62f, normalize(Vec3(6.f, -2.f, 1.f)));
}

void scene_update()
{
	THINGS_FOREACH(&scene.units)
		unit_update(it);

	THINGS_FOREACH(&scene.projectiles)
		projectile_update(it);

#if CLIENT
	THINGS_FOREACH(&scene.health_bars)
		health_bar_update(it);

	scene_draw_sphere(scene.sphere.origin, scene.sphere.radius, Color_Red);
	scene_draw_box(scene.aligned_box.position, scene.aligned_box.size, Quat_Identity, Color_Red);
	scene_draw_box(scene.box.position, scene.box.size, scene.box.orientation, Color_Red);

#endif
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

Unit_Handle scene_unit_handle(Unit* unit)
{
	return thing_get_handle(&scene.units, unit);
}

Unit_Handle scene_unit_handle(u32 id)
{
	return thing_get_handle_at(&scene.units, id);
}

Unit* scene_get_unit(const Unit_Handle& handle)
{
	return thing_resolve(&scene.units, handle);
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


Projectile* scene_make_projectile(const Unit_Handle& owner, u32 proj_id, const Vec2& origin, const Vec2& direction)
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

// Drawing primitives
void scene_draw_line(const Vec3& from, const Vec3& to, const Vec4& color, float duration)
{
	primitive_draw_line(&scene.primitive_manager, from, to, color, duration);
}

void scene_draw_point(const Vec3& position, const Vec4& color, float duration)
{
	primitive_draw_point(&scene.primitive_manager, position, color, duration);
}

void scene_draw_sphere(const Vec3& origin, float radius, const Vec4& color, float duration)
{
	primitive_draw_sphere(&scene.primitive_manager, origin, radius, color, duration);
}

void scene_draw_box(const Vec3& position, const Vec3& size, const Quat& orientation, const Vec4& color, float duration)
{
	primitive_draw_box(&scene.primitive_manager, position, size, orientation, color, duration);
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

		primitives_render(&scene.primitive_manager, state);
	}
}
#endif