#include "Scene.h"
#include "Core/Input/Input.h"
#include "Core/Context/Context.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include "Runtime/Unit/Unit.h"
#include "Runtime/Prop/Prop.h"
#include "Runtime/Weapon/Weapon.h"
#include "Runtime/Game/Projectile.h"
#include "Runtime/Render/HealthBar.h"
#include "Runtime/Render/Drawable.h"
#include "Runtime/Render/Billboard.h"
#include "Runtime/Effect/LineDrawer.h"
#include "Runtime/Fx/Fx.h"

Scene scene;

void scene_init()
{
	thing_array_init(&scene.units, MAX_UNITS);
	thing_array_init(&scene.weapons, MAX_WEAPONS);
	thing_array_init(&scene.projectiles, MAX_PROJECTILES);
	thing_array_init(&scene.props, MAX_PROPS);

#if CLIENT
	thing_array_init(&scene.drawables, MAX_DRAWABLES);
	thing_array_init(&scene.billboards, MAX_BILLBOARDS);
	thing_array_init(&scene.line_drawers, MAX_LINE_DRAWERS);
	thing_array_init(&scene.health_bars, MAX_HEALTH_BARS);

	primitives_init(&scene.primitive_manager);
	fx_init();
#endif
}

void scene_update()
{
	THINGS_FOREACH(&scene.units)
		unit_update(it);

	THINGS_FOREACH(&scene.weapons)
		weapon_update(it);

	THINGS_FOREACH(&scene.projectiles)
		projectile_update(it);

#if CLIENT
	THINGS_FOREACH(&scene.health_bars)
		health_bar_update(it);

	fx_update();
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

Weapon* scene_make_weapon(Unit* owner)
{
	Weapon* weapon = thing_add(&scene.weapons);
	weapon_init(weapon, owner);

	return weapon;
}

void scene_destroy_weapon(Weapon* weapon)
{
	weapon_free(weapon);
	thing_remove(&scene.weapons, weapon);
}

Projectile* scene_make_projectile(const Unit_Handle& owner, const Vec2& origin, const Vec2& direction)
{
	Projectile* projectile = thing_add(&scene.projectiles);
	projectile_init(projectile, owner, origin, direction);

	return projectile;
}

void scene_destroy_projectile(Projectile* projectile)
{
	projectile_free(projectile);
	thing_remove(&scene.projectiles, projectile);
}

Prop* scene_make_prop(const char* path)
{
	Prop* prop = thing_add(&scene.props);
	prop_init(prop, path);

	return prop;
}

void scene_destroy_prop(Prop* prop)
{
	prop_free(prop);
	thing_remove(&scene.props, prop);
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

void scene_draw_convex_shape(const Convex_Shape* shape)
{
	u32 index_offset = 0;
	for(u32 face_index=0; face_index<shape->num_faces; ++face_index)
	{
		Vec3 min = shape->vertices[shape->indicies[index_offset]];
		Vec3 max = shape->vertices[shape->indicies[index_offset]];

		// Draw all the edges
		Convex_Shape_Face& face = shape->faces[face_index];
		for(u32 i=1; i<face.vert_count; ++i)
		{
			Vec3 a = shape->vertices[shape->indicies[index_offset + i - 1]];
			Vec3 b = shape->vertices[shape->indicies[index_offset + i]];

			scene_draw_line(a, b);

			// While we're here, save min and max
			min = component_min(min, a);
			min = component_min(min, b);
			max = component_max(max, a);
			max = component_max(max, b);
		}

		// Draw the normal
		Vec3 middle = min + (max - min) * 0.5f;
		scene_draw_line(middle, middle + face.normal * 0.2f, Color_Red);

		index_offset += face.vert_count;
	}
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

		fx_render(state);
	}
}
#endif