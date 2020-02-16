#include "AssaultRifle.h"
#include "Weapon.h"
#include "Runtime/Unit/Unit.h"
#include "Runtime/Online/Channel.h"
#include "Runtime/Game/Scene.h"
#include "Runtime/Weapon/Projectile/Bullet.h"

enum Assault_Rifle_Event
{
	EVENT_Shoot,
};

void assault_rifle_event_proc(Channel* chnl, Online_User* src)
{
	Assault_Rifle* rifle = (Assault_Rifle*)chnl->user_ptr;

	u8 event_type;
	channel_read(chnl, &event_type);

	switch(event_type)
	{
		case EVENT_Shoot:
		{
			Vec2 origin;
			Vec2 direction;

			channel_read(chnl, &origin);
			channel_read(chnl, &direction);

			Bullet_Params bullet_params;
			bullet_params.origin = origin;
			bullet_params.direction = direction;
			bullet_params.size = 0.2f;
			bullet_params.speed = 72.f;

			scene_make_bullet(rifle->weapon->owner, bullet_params);

#if SERVER
			if (src != nullptr)
				server_rebroadcast_last(chnl, true);
#endif
			break;
		}
	}
}

void assault_rifle_init(Assault_Rifle* rifle, Weapon* weapon, const Weapon_Attributes& attributes)
{
	Unit* owner_unit = scene_get_unit(weapon->owner);
	assert(owner_unit);

	rifle->weapon = weapon;
	rifle->channel = channel_open("ASRF", owner_unit->id, assault_rifle_event_proc);
	rifle->channel->user_ptr = rifle;
}

void assault_rifle_free(Assault_Rifle* rifle)
{
	channel_close(rifle->channel);
}

void assault_rifle_update(Assault_Rifle* rifle)
{
	if (rifle->trigger_held)
	{
		float fire_rate_delta = 1.f / assault_rifle_fire_rate;

		// OK to fire
		if (rifle->last_fire_time + fire_rate_delta < time_elapsed())
		{
			Vec2 direction = normalize(rifle->fire_target - rifle->weapon->position);

			// Apply spread
			float spread_radians = radians(rifle->spread_degrees);
			direction = rotate_vector(direction, random_float(-spread_radians, spread_radians));
			Vec2 origin = rifle->weapon->position + direction * 0.5f;

			channel_reset(rifle->channel);
			channel_write_u8(rifle->channel, EVENT_Shoot);
			channel_write_vec2(rifle->channel, origin);
			channel_write_vec2(rifle->channel, direction);
			channel_broadcast(rifle->channel, true);

			rifle->last_fire_time = time_elapsed();
			rifle->spread_degrees += assault_rifle_spread_accum;
		}

		rifle->trigger_held = false;
	}

	rifle->spread_degrees -= rifle->spread_degrees * assault_rifle_spread_decrease * time_delta();
}

void assault_rifle_fire(Assault_Rifle* rifle, const Vec2& target)
{
	rifle->trigger_held = true;
	rifle->fire_target = target;
}