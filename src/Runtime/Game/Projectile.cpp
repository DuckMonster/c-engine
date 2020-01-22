#include "Projectile.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Collision/HitTest.h"
#include "Runtime/Render/Drawable.h"
#include "Runtime/Effect/LineDrawer.h"
#include "Runtime/Game/Game.h"
#include "Runtime/Game/Scene.h"
#include "Runtime/Game/SceneQuery.h"
#include "Runtime/Unit/Unit.h"
#include "Runtime/Fx/Fx.h"

void projectile_init(Projectile* projectile, const Unit_Handle& owner, const Vec2& position, const Vec2& direction)
{
	projectile->owner = owner;

	projectile->position = position;
	projectile->direction = direction;

#if CLIENT
	projectile->drawable = scene_make_drawable(mesh_load("Mesh/sphere.fbx"), material_load("Material/bullet.mat"));
	projectile->drawable->transform = mat_position_rotation_scale(
		Vec3(position, 0.5f), quat_from_x(Vec3(direction, 0.f)), projectile->size
	);

	projectile->line_drawer = scene_make_line_drawer(Vec3(position, 0.5f));
#endif

	projectile->lifetime = 0.f;
}

void projectile_free(Projectile* projectile)
{
#if CLIENT
	if (projectile->drawable)
	{
		scene_destroy_drawable(projectile->drawable);
		projectile->drawable = nullptr;
	}

	projectile->line_drawer->should_destroy = true;
#endif
}

void projectile_update(Projectile* projectile)
{
	Unit* owner = scene_get_unit(projectile->owner);

	// Movement ray from (before moving)
	Line_Trace move_trace;
	move_trace.from = Vec3(projectile->position, 0.5f);

	// Do movement
	projectile->position += projectile->direction * projectile->speed * time_delta();

#if CLIENT
	float sphere_scale = saturate(projectile->lifetime / 0.05f);
	projectile->drawable->transform = mat_position_rotation_scale(
		Vec3(projectile->position, 0.5f), quat_from_x(Vec3(projectile->direction, 0.f)), projectile->size * sphere_scale
	);
	projectile->line_drawer->position = Vec3(projectile->position, 0.5f);
#endif

	// Movement line to (after moving)
	move_trace.to = Vec3(projectile->position, 0.5f);

	Scene_Query_Params params;
	params.ignore_unit = owner;

	// Then! Do collision checking to see if we hit something
	Scene_Query_Result query_result = scene_query_line(move_trace, params);
	if (query_result.hit.has_hit)
	{
		Unit* unit = query_result.unit;
		if (unit && owner && unit_has_control(owner))
			unit_hit(unit, projectile->owner, projectile->direction * 20.f);
		else if (!unit)
		{
#if CLIENT
			Vec3 normal = query_result.hit.normal;
			Vec3 pos = query_result.hit.position + normal * 0.1f;
			Vec3 velocity = reflect(Vec3(projectile->direction, 0.f) * projectile->speed, normal) * 0.2f;

			// If we hit a prop, spawn some neat projectiles!
			Fx_Particle_Spawn_Params params;
			params.num_particles = 10;
			params.position = query_result.hit.position + query_result.hit.normal * 0.2f;
			params.position_radius = 0.1f;
			params.velocity = velocity;
			params.velocity_cone_angle = 40.f;
			params.velocity_scale_variance = 0.9f;

			params.drag_min = 3.5f;
			params.drag_max = 4.5f;
			params.gravity_min = 4.f;
			params.gravity_max = 10.f;

			params.color_min = Color_Dark_Gray;
			params.color_max = Color_White;
			fx_make_particle(params);
#endif
		}

		scene_destroy_projectile(projectile);
		return;
	}

	// Lifetime
	projectile->lifetime += time_delta();
	if (projectile->lifetime > 5.f)
	{
		scene_destroy_projectile(projectile);
		return;
	}
}