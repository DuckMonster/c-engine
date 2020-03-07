#include "Weapon.h"
#include "Core/Input/Input.h"
#include "Engine/Graphics/SpriteSheet.h"
#include "Runtime/Game/Game.h"
#include "Runtime/Game/Scene.h"
#include "Runtime/Unit/Unit.h"
#include "Runtime/Render/Billboard.h"
#include "Runtime/Fx/Fx.h"
#include "Runtime/Fx/FxResource.h"
#include "Runtime/Weapon/WeaponType.h"
#include "Runtime/Weapon/Projectile/Bullet.h"
#include "Runtime/Online/Channel.h"
#include "Pistol.h"
#include "AssaultRifle.h"

enum Weapon_Event
{
	EVENT_Fire_Bullet,
};

Vec3 add_cone_rotation(const Vec3& vec, float cone_half_angle)
{
	return angle_axis(radians(random_float(-cone_half_angle, cone_half_angle)), Vec3_Z) * vec;
}

void weapon_event_proc(Channel* chnl, Online_User* src)
{
	Weapon* weapon = (Weapon*)chnl->user_ptr;

	u8 event_type;
	channel_read(chnl, &event_type);

	switch(event_type)
	{
		case EVENT_Fire_Bullet:
		{
			u8 num_bullets;
			channel_read(chnl, &num_bullets);

			Vec3 target;
			channel_read(chnl, &target);

			Vec3 shoot_direction = normalize(target - weapon->position);

			for(u8 i=0; i<num_bullets; ++i)
			{
				Bullet_Params params;
				channel_read_t(chnl, &params);

				scene_make_bullet(weapon->owner, params);

				// If we're only shooting one bullet, make that the muzzle flash direction
				if (num_bullets == 1)
					shoot_direction = params.direction;
			}

			weapon->recoil_angle += weapon->type_data->recoil_gain;

#if CLIENT
			Vec3 direction = normalize(target - weapon->position);
			Fx_Params params = fx_params_make(
				weapon->position + direction * weapon->type_data->muzzle_offset,
				direction,
				weapon->type_data->muzzle_fx_scale
			);

			fx_make_res(
				fx_resource_load("Fx/muzzle_flash.dat"),
				params
			);

			weapon_reset_offset(weapon);
			weapon_add_velocity(weapon, -direction * 6.f, 5.f);
#endif

#if SERVER
			if (src != nullptr)
				channel_rebroadcast_last(chnl, true);
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
	weapon->billboard->anchor = Vec2(0.f, 0.5f);
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
	channel_reset(weapon->channel);
	channel_write_u8(weapon->channel, EVENT_Fire_Bullet);
	channel_write_u8(weapon->channel, weapon->type_data->projectile_num);
	channel_write_vec3(weapon->channel, target);

	for(u32 i=0; i<weapon->type_data->projectile_num; ++i)
	{
		Vec3 direction = normalize(target - weapon->position);
		direction = add_cone_rotation(direction, weapon->recoil_angle + weapon->type_data->projectile_spread);

		float speed = weapon->type_data->projectile_speed + weapon->type_data->projectile_speed_variance * random_float(-1.f, 1.f);

		Bullet_Params params;
		params.direction = direction;
		params.origin = weapon->position + params.direction * weapon->type_data->muzzle_offset;
		params.damage = weapon->type_data->damage;
		params.size = weapon->type_data->projectile_size;
		params.speed = speed;
		params.drag = weapon->type_data->projectile_drag;
		params.gravity = weapon->type_data->projectile_gravity;

		channel_write_t(weapon->channel, params);
	}

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