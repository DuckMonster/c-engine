#include "Weapon.h"
#include "Core/Input/Input.h"
#include "Engine/Graphics/SpriteSheet.h"
#include "Runtime/Game/Game.h"
#include "Runtime/Game/Scene.h"
#include "Runtime/Unit/Unit.h"
#include "Runtime/Render/Billboard.h"
#include "Runtime/Fx/Fx.h"

void weapon_init(Weapon* weapon, Unit* owner)
{
	weapon->owner = scene_unit_handle(owner);
	weapon->position = owner->position;

#if CLIENT
	weapon->billboard = scene_make_billboard(sprite_sheet_load("Sprite/weapon_sheet.dat"));
	weapon->billboard->position = Vec3(owner->position, 0.5f);
	weapon->billboard->rotation_type = ROTATION_World_Direction;
#endif
}

void weapon_free(Weapon* weapon)
{
#if CLIENT
	scene_destroy_billboard(weapon->billboard);
#endif
}

void weapon_update(Weapon* weapon)
{
	Unit* owner = scene_get_unit(weapon->owner);
	assert(owner);

	Vec2 target = owner->position + owner->aim_direction * weapon_hold_distance;
	weapon->position = lerp(weapon->position, target, weapon_interp_speed * time_delta());

#if CLIENT
	// Move billboard
	weapon->billboard->position = Vec3(weapon->position, 0.5f);

	/* Shoot animation stuff */
	weapon->offset += weapon->offset_velocity * time_delta();
	weapon->offset_velocity -= weapon->offset * weapon_offset_acceleration * time_delta();
	weapon->offset_velocity -= weapon->offset_velocity * weapon_offset_drag * time_delta();

	weapon->angle_offset += weapon->angle_offset_velocity * time_delta();
	weapon->angle_offset_velocity -= weapon->angle_offset * weapon_angle_offset_acceleration * time_delta();
	weapon->angle_offset_velocity -= weapon->angle_offset_velocity * weapon_angle_offset_drag * time_delta();

	weapon->billboard->position += Vec3(weapon->offset, 0.f);

	// Rotation
	Quat weapon_quat = quat_from_x(Vec3(owner->aim_direction, 0.f));
	Quat recoil_quat = angle_axis(weapon->angle_offset, -Vec3_Y);

	Vec3 aim_forward = quat_x(weapon_quat * recoil_quat);
	weapon->billboard->rotation_direction = aim_forward;

	if (input_key_down(Key::L))
	{
		weapon->billboard->rotation_type = ROTATION_Angle;
		weapon->billboard->rotation_angle = 1.f;
	}
#endif
}

void weapon_shoot(Weapon* weapon, const Vec2& target)
{
	Vec2 direction = normalize(target - weapon->position);
	Vec2 origin = weapon->position + direction * 0.4f;
	scene_make_projectile(weapon->owner, origin, direction);

#if CLIENT
	float impulse_strength = random_float(0.4f, 1.f);
	/* Shoot animation */
	// First reset the animation
	weapon->offset = -direction * 0.2f * impulse_strength;
	weapon->angle_offset = 0.5f * impulse_strength;

	weapon->offset_velocity = -direction * weapon_shoot_impulse * impulse_strength;
	weapon->angle_offset_velocity = weapon_shoot_angle_impulse * impulse_strength;

	/* Spawn some pretty particles! */
	Fx_Particle_Spawn_Params params;
	params.num_particles = 6;
	params.position = Vec3(origin, 0.5f);
	params.position_radius = 0.1f;
	params.velocity = Vec3(direction, 0.f) * 4.f;
	params.velocity_cone_angle = 20.f;
	params.velocity_scale_variance = 0.9f;

	params.drag_min = 1.5f;
	params.drag_max = 5.5f;
	params.gravity_min = -20.f;
	params.gravity_max = -9.f;

	params.color_min = Color_Dark_Gray;
	params.color_max = Color_Light_Gray;

	params.lifetime_min = 0.4f;
	params.lifetime_max = 0.8f;
	fx_make_particle(params);
#endif
}