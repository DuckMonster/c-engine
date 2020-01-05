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

	// Movement ray start (before moving)
	Line move_line;
	move_line.start = Vec3(projectile->position, 0.5f);

	// Do movement
	projectile->position += projectile->direction * projectile->speed * time_delta();

#if CLIENT
	float sphere_scale = saturate(projectile->lifetime / 0.05f);
	projectile->drawable->transform = mat_position_rotation_scale(
		Vec3(projectile->position, 0.5f), quat_from_x(Vec3(projectile->direction, 0.f)), projectile->size * sphere_scale
	);
	projectile->line_drawer->position = Vec3(projectile->position, 0.5f);
#endif

	// Movement line end (after moving)
	move_line.end = Vec3(projectile->position, 0.5f);

	// Then! Do collision checking to see if we hit something
	Scene_Query_Result query_result = scene_query_line(move_line);
	if (query_result.hit.has_hit)
	{
		Unit* unit = query_result.unit;
		if (unit && owner && unit_has_control(owner))
			unit_hit(unit, projectile->owner, projectile->direction * 20.f);

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