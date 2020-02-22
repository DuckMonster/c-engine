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

#if CLIENT
static Vec3 rotate_cone(const Vec3& vec, float half_angle)
{
	Quat result =
		quat_from_x(vec) *
		angle_axis(radians(random_float(-half_angle, half_angle)), Vec3_Z) *
		angle_axis(random_float(TAU), Vec3_X);

	return quat_x(result);
}

static void make_hit_fx(const Vec3& velocity, const Hit_Result& hit)
{
	Vec3 direction = normalize(velocity);
	//direction = reflect(direction, hit.normal);
	direction = -direction;

	// Main spike
	{
		Fx_Spike_Params spike;
		spike.from = hit.position - direction * 0.4f;
		spike.to = hit.position + direction * 0.6f;
		spike.size = 0.4f;
		spike.duration = 0.35f;
		spike.center_alpha = 0.3f;
		spike.translate_delta = 0.1f;
		spike.to_delta = 1.7f;
		spike.move_exponent = 6.f;
		spike.size_exponent = 6.f;

		fx_make_spike(spike);
	}

	// Spread spikes
	for(u32 i=0; i<8; ++i)
	{
		Vec3 spread_direction = rotate_cone(direction, 40.f);

		Fx_Spike_Params spike;
		spike.from = hit.position + spread_direction * 0.1f;
		spike.to = hit.position + spread_direction * 0.5f;
		spike.size = 0.15f;
		spike.duration = 0.1f;
		spike.center_alpha = 0.3f;
		spike.to_delta = 0.5f;

		//fx_make_spike(spike);
	}
}
#endif

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
		make_hit_fx(bullet->velocity, query_result.hit);
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