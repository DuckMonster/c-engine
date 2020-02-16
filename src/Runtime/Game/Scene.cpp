#include "Scene.h"
#include "Core/Input/Input.h"
#include "Core/Context/Context.h"
#include "Core/Debug/Profiling.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include "Runtime/Unit/Unit.h"
#include "Runtime/Prop/Prop.h"
#include "Runtime/Weapon/Weapon.h"
#include "Runtime/Weapon/ItemDrop.h"
#include "Runtime/Weapon/Projectile/Bullet.h"
#include "Runtime/Game/Grass.h"
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
	thing_array_init(&scene.bullets, MAX_BULLETS);
	thing_array_init(&scene.props, MAX_PROPS);
	thing_array_init(&scene.drops, MAX_ITEM_DROPS);

#if CLIENT
	thing_array_init(&scene.drawables, MAX_DRAWABLES);
	thing_array_init(&scene.billboards, MAX_BILLBOARDS);
	thing_array_init(&scene.line_drawers, MAX_LINE_DRAWERS);
	thing_array_init(&scene.health_bars, MAX_HEALTH_BARS);

	scene.grass = new Grass();
	grass_init(scene.grass);

	primitives_init(&scene.primitive_manager);
	fx_init();
#endif
}

void scene_update()
{
	pix_event_scope("SceneUpdate");

	THINGS_FOREACH(&scene.units)
		unit_update(it);

	THINGS_FOREACH(&scene.weapons)
		weapon_update(it);

	THINGS_FOREACH(&scene.bullets)
		bullet_update(it);

#if CLIENT
	THINGS_FOREACH(&scene.health_bars)
		health_bar_update(it);

	fx_update();
#endif
}

Unit* scene_make_unit(i32 id, const Vec3& position)
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

Weapon* scene_make_weapon(Unit* owner, const Weapon_Instance& instance)
{
	Weapon* weapon = thing_add(&scene.weapons);
	weapon_init(weapon, owner, instance);

	return weapon;
}

void scene_destroy_weapon(Weapon* weapon)
{
	weapon_free(weapon);
	thing_remove(&scene.weapons, weapon);
}

Bullet* scene_make_bullet(const Unit_Handle& owner, const Bullet_Params& params)
{
	Bullet* bullet = thing_add(&scene.bullets);
	bullet_init(bullet, owner, params);

	return bullet;
}

void scene_destroy_bullet(Bullet* bullet)
{
	bullet_free(bullet);
	thing_remove(&scene.bullets, bullet);
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

Item_Drop* scene_make_item_drop(const Vec3& position, const Weapon_Instance& weapon)
{
	Item_Drop* drop = thing_add(&scene.drops);
	item_drop_init(drop, position, weapon);

	return drop;
}

void scene_destroy_item_drop(Item_Drop* drop)
{
	item_drop_free(drop);
	thing_remove(&scene.drops, drop);
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
	for(u32 tri_index = 0; tri_index < shape->num_tris; ++tri_index)
	{
		Triangle& triangle = shape->triangles[tri_index];

		// Draw all the edges
		for(u32 i=0; i<3; ++i)
		{
			int i_next = (i + 1) % 3;
			scene_draw_line(triangle.verts[i], triangle.verts[i_next]);
		}

		// Draw the normal
		scene_draw_line(triangle.centroid, triangle.centroid + triangle.normal * 0.2f, Color_Red);
	}
}

void scene_render(const Render_State& state)
{
	pix_event_scope("SceneRender");

	/* Drawables */
	THINGS_FOREACH(&scene.drawables)
		drawable_render(it, state);

	THINGS_FOREACH(&scene.billboards)
		billboard_render(it, state);

	if (state.current_pass == PASS_Game)
	{
		THINGS_FOREACH(&scene.drops)
			item_drop_render(it, state);

		THINGS_FOREACH(&scene.line_drawers)
			line_drawer_render(it, state);

		THINGS_FOREACH(&scene.health_bars)
			health_bar_render(it, state);

		grass_render(scene.grass, state);
		primitives_render(&scene.primitive_manager, state);

		fx_render(state);
	}
}
#endif