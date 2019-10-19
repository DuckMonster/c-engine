#include "Projectile.h"
#include "Engine/Render/Drawable.h"
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
	projectile->drawable = drawable_load("Mesh/sphere.fbx", "Material/bullet.mat");
	projectile->drawable->transform = mat_position_rotation_scale(
		Vec3(position, 0.5f), quat_from_x(Vec3(direction, 0.f)), Vec3(1.f, projectile->size, projectile->size)
	);
	projectile->line_drawer = line_drawer_make();
	projectile->line_drawer->position = Vec3(position, 0.5f);
#endif

	projectile->lifetime = 5.f;

	return projectile;
}

void projectile_destroy(Projectile* projectile)
{
#if CLIENT
	drawable_destroy(projectile->drawable);
	projectile->drawable = nullptr;

	line_drawer_destroy(projectile->line_drawer);
	projectile->line_drawer = nullptr;
#endif

	splist_remove(&scene.projectiles, projectile);
}

void projectile_update(Projectile* projectile)
{
	projectile->position += projectile->direction * projectile->speed * time_delta();

#if CLIENT
	projectile->drawable->transform[3] = Vec4(projectile->position, 0.5f, 1.f);
	projectile->line_drawer->position = Vec3(projectile->position, 0.5f);
#endif

	projectile->lifetime -= time_delta();
	if (projectile->lifetime < 0.f)
	{
		projectile_destroy(projectile);
		return;
	}
}