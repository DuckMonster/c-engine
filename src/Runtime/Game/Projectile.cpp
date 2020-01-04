#include "Projectile.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include "Runtime/Render/Drawable.h"
#include "Runtime/Effect/LineDrawer.h"
#include "Runtime/Game/Game.h"
#include "Runtime/Game/Scene.h"
#include "Runtime/Unit/Unit.h"

void projectile_init(Projectile* projectile, const Unit_Handle& owner, u32 proj_id, const Vec2& position, const Vec2& direction)
{
	projectile->proj_id = proj_id;
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

	// Movement ray (before moving)
	Ray move_ray;
	move_ray.origin = Vec3(projectile->position, 0.f);
	move_ray.direction = Vec3(projectile->direction, 0.f);

	// Do movement
	projectile->position += projectile->direction * projectile->speed * time_delta();

	// Then! Do collision checking to see if we hit something
	Unit* hit_unit = nullptr;
	THINGS_FOREACH(&scene.units)
	{
		Unit* unit = it;
		if (unit == owner)
			continue;

		float intersect_time = 0.f;
		if (ray_sphere_intersect(move_ray, Vec3(unit->position, 0.f), 1.f, &intersect_time))
		{
			if (intersect_time < projectile->speed * time_delta())
			{
				Vec2 diff = constrain_to_direction(unit->position - projectile->position, projectile->direction);
				projectile->position += diff;

				hit_unit = unit;
				break;
			}
		}
	}

#if CLIENT
	float sphere_scale = saturate(projectile->lifetime / 0.05f);
	projectile->drawable->transform = mat_position_rotation_scale(
		Vec3(projectile->position, 0.5f), quat_from_x(Vec3(projectile->direction, 0.f)), projectile->size * sphere_scale
	);
	projectile->line_drawer->position = Vec3(projectile->position, 0.5f);
#endif

	if (hit_unit)
	{
		if (owner && unit_has_control(owner))
			unit_hit(hit_unit, projectile->owner, projectile->direction * 20.f);

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