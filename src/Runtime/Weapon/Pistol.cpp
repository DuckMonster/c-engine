#include "Pistol.h"
#include "Weapon.h"
#include "Runtime/Game/Scene.h"
#include "Runtime/Unit/Unit.h"
#include "Runtime/Online/Channel.h"
#include "Runtime/Fx/Fx.h"
#include "Runtime/Weapon/Projectile/Bullet.h"

enum Pistol_Event
{
	EVENT_Shoot,
};

void pistol_event_proc(Channel* chnl, Online_User* src)
{
	Pistol* pistol = (Pistol*)chnl->user_ptr;

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

			pistol_fire(pistol, origin, direction);
			break;
		}
	}
}

void pistol_init(Pistol* pistol, Weapon* weapon, const Weapon_Attributes& attributes)
{
	Unit* owner_unit = scene_get_unit(weapon->owner);
	assert(owner_unit);

	pistol->weapon = weapon;
	pistol->channel = channel_open("PSTL", owner_unit->id, pistol_event_proc);
	pistol->channel->user_ptr = pistol;
}

void pistol_free(Pistol* pistol)
{
	channel_close(pistol->channel);
}

void pistol_fire_local(Pistol* pistol, const Vec2& target)
{
	Vec2 direction = normalize(target - pistol->weapon->position);
	Vec2 origin = pistol->weapon->position + direction * 0.5f;

	channel_reset(pistol->channel);
	channel_write_u8(pistol->channel, WEAPON_Pistol);
	channel_write_vec2(pistol->channel, origin);
	channel_write_vec2(pistol->channel, direction);
	channel_broadcast(pistol->channel, true);
}

void pistol_fire(Pistol* pistol, const Vec2& origin, const Vec2& direction)
{
	Bullet_Params bullet_params;
	bullet_params.origin = origin;
	bullet_params.direction = direction;
	bullet_params.size = 0.2f;
	bullet_params.speed = 85.f;

	scene_make_bullet(pistol->weapon->owner, bullet_params);

#if CLIENT
	float impulse_strength = random_float(0.4f, 1.f);
	/* Shoot animation */
	// First reset the animation
	weapon_reset_offset(pistol->weapon);
	weapon_add_velocity(pistol->weapon, -direction * 8.f * impulse_strength, 20.f * impulse_strength); 

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