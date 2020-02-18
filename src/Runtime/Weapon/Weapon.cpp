#include "Weapon.h"
#include "Core/Input/Input.h"
#include "Engine/Graphics/SpriteSheet.h"
#include "Runtime/Game/Game.h"
#include "Runtime/Game/Scene.h"
#include "Runtime/Unit/Unit.h"
#include "Runtime/Render/Billboard.h"
#include "Runtime/Fx/Fx.h"
#include "Runtime/Weapon/WeaponType.h"
#include "Runtime/Weapon/Projectile/Bullet.h"
#include "Runtime/Online/Channel.h"
#include "Pistol.h"
#include "AssaultRifle.h"

enum Weapon_Event
{
	EVENT_Fire_Bullet,
};

void weapon_event_proc(Channel* chnl, Online_User* src)
{
	Weapon* weapon = (Weapon*)chnl->user_ptr;

	u8 event_type;
	channel_read(chnl, &event_type);

	switch(event_type)
	{
		case EVENT_Fire_Bullet:
		{
			Bullet_Params params;
			channel_read_t(chnl, &params);

			scene_make_bullet(weapon->owner, params);

			weapon->recoil_angle += weapon->type_data->recoil_gain;

#if CLIENT
			weapon_reset_offset(weapon);
			weapon_add_velocity(weapon, -params.direction * 6.f, 5.f);
#endif
			break;
		}
	}
}

void weapon_init(Weapon* weapon, Unit* owner, const Weapon_Instance& instance)
{
	weapon->owner = scene_unit_handle(owner);
	weapon->position = owner->position;
	weapon->instance = instance;
	weapon->type_data = weapon_get_type_data(instance.type);

	weapon->channel = channel_open("WPN", owner->id, weapon_event_proc);
	weapon->channel->user_ptr = weapon;

#if CLIENT
	const Weapon_Type_Data* type_data = weapon_get_type_data(instance.type);

	weapon->billboard = scene_make_billboard(sprite_sheet_load("Sprite/weapon_sheet.dat"));
	weapon->billboard->position = owner->position;
	weapon->billboard->rotation_type = ROTATION_World_Direction;
	weapon->billboard->tile_x = type_data->tile_x;
	weapon->billboard->tile_y = type_data->tile_y;
#endif
}

void weapon_free(Weapon* weapon)
{
	channel_close(weapon->channel);

#if CLIENT
	scene_destroy_billboard(weapon->billboard);
#endif
}

void weapon_update(Weapon* weapon)
{
	Unit* owner = scene_get_unit(weapon->owner);
	assert(owner);

	Vec3 target = unit_center(owner) + owner->aim_direction * weapon_hold_distance;
	weapon->position = lerp(weapon->position, target, weapon_interp_speed * time_delta());

	// Decay recoil
	weapon->recoil_angle -= weapon->recoil_angle * weapon->type_data->recoil_decay * time_delta();

#if CLIENT
	// Move billboard
	weapon->billboard->position = weapon->position;

	/* Shoot animation stuff */
	weapon->offset += weapon->offset_velocity * time_delta();
	weapon->offset_velocity -= weapon->offset * weapon_offset_acceleration * time_delta();
	weapon->offset_velocity -= weapon->offset_velocity * weapon_offset_drag * time_delta();

	weapon->angle_offset += weapon->angle_offset_velocity * time_delta();
	weapon->angle_offset_velocity -= weapon->angle_offset * weapon_angle_offset_acceleration * time_delta();
	weapon->angle_offset_velocity -= weapon->angle_offset_velocity * weapon_angle_offset_drag * time_delta();

	weapon->billboard->position += weapon->offset;

	// Rotation
	Quat weapon_quat = quat_from_x(owner->aim_direction);
	Quat recoil_quat = angle_axis(weapon->angle_offset, -Vec3_Y);

	Vec3 aim_forward = quat_x(weapon_quat * recoil_quat);
	weapon->billboard->rotation_direction = aim_forward;
#endif
}

void weapon_fire(Weapon* weapon, const Vec3& target)
{
	float fire_recoil = radians(random_float(-weapon->recoil_angle, weapon->recoil_angle) * 0.5f);

	Quat recoil_quat;
	recoil_quat =
		quat_from_x(target - weapon->position) *
		angle_axis(TAU, Vec3_X) *
		angle_axis(fire_recoil, Vec3_Z);

	channel_reset(weapon->channel);
	channel_write_u8(weapon->channel, EVENT_Fire_Bullet);

	Bullet_Params params;
	params.direction = quat_x(recoil_quat);
	params.origin = weapon->position + params.direction * 0.4f;
	params.damage = weapon->type_data->damage;

	channel_write_t(weapon->channel, params);
	channel_broadcast(weapon->channel, true);

	weapon->last_fire_time = time_elapsed();
}

void weapon_hold_trigger(Weapon* weapon, const Vec3& target)
{
	switch(weapon->type_data->fire_mode)
	{
		case FIREMODE_Press:
			if (!weapon->trigger_held)
				weapon_fire(weapon, target);

			break;

		case FIREMODE_Auto:
			if (time_elapsed() > weapon->last_fire_time + (1.f / weapon->type_data->fire_rate))
				weapon_fire(weapon, target);

			break;
	}

	weapon->trigger_held = true;
}

void weapon_release_trigger(Weapon* weapon, const Vec3& target)
{
	weapon->trigger_held = false;
}

#if CLIENT
void weapon_reset_offset(Weapon* weapon)
{
	weapon->offset = Vec3();
	weapon->angle_offset = 0.f;
}

void weapon_add_velocity(Weapon* weapon, const Vec3& linear_velocity, float angular_velocity)
{
	weapon->offset_velocity += linear_velocity;
	weapon->angle_offset_velocity += angular_velocity;
}
#endif