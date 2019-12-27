#include "Projectile.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include "Runtime/Render/Drawable.h"
#include "Runtime/Effect/LineDrawer.h"
#include "Runtime/Game/Scene.h"
#include "Runtime/Unit/Unit.h"

Projectile* projectile_spawn(Unit* owner, u32 proj_id, const Vec2& position, const Vec2& direction)
{
	Projectile* projectile = splist_add(&scene.projectiles);
	projectile->id = proj_id;
	projectile->owner = owner;

	projectile->position = position;
	projectile->direction = direction;

#if CLIENT
	projectile->drawable = scene_make_drawable();
	drawable_init(projectile->drawable, mesh_load("Mesh/sphere.fbx"), material_load("Material/bullet.mat"));

	projectile->drawable->transform = mat_position_rotation_scale(
		Vec3(position, 0.5f), quat_from_x(Vec3(direction, 0.f)), projectile->size
	);
	projectile->line_drawer = line_drawer_make();
	projectile->line_drawer->position = Vec3(position, 0.5f);
#endif

	projectile->lifetime = 0.f;

	return projectile;
}

void projectile_fade_out(Projectile* projectile)
{
#if CLIENT
	scene_destroy_drawable(projectile->drawable);
	projectile->drawable = nullptr;
#endif

	projectile->is_fading = true;
}

void projectile_destroy(Projectile* projectile)
{
#if CLIENT
	if (projectile->drawable)
	{
		scene_destroy_drawable(projectile->drawable);
		projectile->drawable = nullptr;
	}

	line_drawer_destroy(projectile->line_drawer);
	projectile->line_drawer = nullptr;
#endif

	*projectile = Projectile();
	splist_remove(&scene.projectiles, projectile);
}

void projectiles_update()
{
	Projectile* projectile = nullptr;
	SPLIST_FOREACH(&scene.projectiles, projectile)
	{
		if (projectile->is_fading)
		{
			projectile->fade_timer -= time_delta();
			if (projectile->fade_timer < 0.f)
				projectile_destroy(projectile);

			continue;
		}

		// Movement ray (before moving)
		Ray move_ray;
		move_ray.origin = Vec3(projectile->position, 0.f);
		move_ray.direction = Vec3(projectile->direction, 0.f);

		// Do movement
		projectile->position += projectile->direction * projectile->speed * time_delta();

#if CLIENT
		float sphere_scale = saturate(projectile->lifetime / 0.05f);
		projectile->drawable->transform = mat_position_rotation_scale(
			Vec3(projectile->position, 0.5f), quat_from_x(Vec3(projectile->direction, 0.f)), projectile->size * sphere_scale
		);
		projectile->line_drawer->position = Vec3(projectile->position, 0.5f);
#endif

		// Then! Do collision checking to see if we hit something
		Unit* hit_unit = nullptr;
		for(u32 i=0; i<MAX_UNITS; ++i)
		{
			Unit* unit = scene.units + i;
			if (!unit->active)
				continue;

			if (unit == projectile->owner)
				continue;

			float intersect_time = 0.f;
			if (ray_sphere_intersect(move_ray, Vec3(unit->position, 0.f), 1.f, &intersect_time))
			{
				if (intersect_time < projectile->speed * time_delta())
				{
					hit_unit = unit;
					break;
				}
			}
		}

		if (hit_unit)
		{
			if (unit_has_control(projectile->owner))
				unit_hit(hit_unit, projectile->direction * 4.f);

			projectile_fade_out(projectile);
			continue;
		}

		// Lifetime
		projectile->lifetime += time_delta();
		if (projectile->lifetime > 5.f)
		{
			projectile_destroy(projectile);
			return;
		}
	}
}