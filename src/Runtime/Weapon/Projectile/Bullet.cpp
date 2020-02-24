#include "Bullet.h"
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

void bullet_init(Bullet* bullet, const Unit_Handle& owner, const Bullet_Params& params)
{
	bullet->owner = owner;

	bullet->position = params.origin;
	bullet->velocity = params.direction * params.speed;
	bullet->params = params;

#if CLIENT
	bullet->drawable = scene_make_drawable(mesh_load("Mesh/sphere.fbx"), material_load("Material/bullet.mat"));
	bullet->drawable->transform = mat_position_rotation_scale(
		params.origin, quat_from_x(params.direction), Vec3_Zero
	);

	bullet->line_drawer = scene_make_line_drawer(params.origin);
#endif

	bullet->lifetime = 0.f;
}

void bullet_free(Bullet* bullet)
{
#if CLIENT
	if (bullet->drawable)
	{
		scene_destroy_drawable(bullet->drawable);
		bullet->drawable = nullptr;
	}

	bullet->line_drawer->should_destroy = true;
#endif
}

void bullet_update(Bullet* bullet)
{
	Unit* owner = scene_get_unit(bullet->owner);

	// Movement ray from (before moving)
	Line_Trace move_trace;
	move_trace.from = bullet->position;

	// Do movement
	bullet->velocity -= bullet->velocity * bullet->params.drag * time_delta();
	bullet->velocity -= Vec3_Z * bullet->params.gravity * time_delta();
	bullet->position += bullet->velocity * time_delta();

#if CLIENT
	float sphere_time_scale = saturate(bullet->lifetime / 0.05f);
	bullet->drawable->transform = mat_position_rotation_scale(
		bullet->position, quat_from_x(bullet->velocity), bullet_scale_base * bullet->params.size * sphere_time_scale
	);
	bullet->line_drawer->position = bullet->position;
	bullet->line_drawer->size = bullet_line_size_base * bullet->params.size;
#endif

	// Movement line to (after moving)
	move_trace.to = bullet->position;

	Scene_Query_Params params;
	params.ignore_unit = owner;

	// Then! Do collision checking to see if we hit something
	Scene_Query_Result query_result = scene_query_line(move_trace, params);
	if (query_result.hit.has_hit)
	{
		Unit* unit = query_result.unit;
		if (unit && owner && unit_has_control(owner))
		{
			Vec3 impulse = normalize(constrain_to_plane(bullet->params.direction, Vec3_Z)) * 20.f;
			unit_hit(unit, bullet->owner, bullet->params.damage, impulse);
		}
		else if (!unit)
		{
#if CLIENT
			Vec3 normal = query_result.hit.normal;
			Vec3 pos = query_result.hit.position + normal * 0.1f;
			Vec3 velocity = reflect(bullet->velocity, normal) * 0.2f;
#endif
		}

#if CLIENT
		bullet->line_drawer->position = query_result.hit.position;
#endif


		scene_destroy_bullet(bullet);
		return;
	}

	// Lifetime
	bullet->lifetime += time_delta();
	if (bullet->lifetime > 5.f)
	{
		scene_destroy_bullet(bullet);
		return;
	}
}