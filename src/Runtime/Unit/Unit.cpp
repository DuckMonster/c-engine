#include "Unit.h"
#include "Engine/Graphics/SpriteSheet.h"
#include "Runtime/Render/Billboard.h"
#include "Runtime/Game/Scene.h"
#include "Runtime/Online/Channel.h"

enum Unit_Event
{
	EVENT_Set_Position,
	EVENT_Shoot,
	EVENT_Hit,
	EVENT_Set_Target,
	EVENT_Set_Aim_Direction,
	EVENT_Set_Health,
	EVENT_Die,
};

void unit_event_proc(Channel* chnl, Online_User* src)
{
	Unit* unit = (Unit*)chnl->user_ptr;

	u8 event_type;
	channel_read(chnl, &event_type);

	switch(event_type)
	{
		case EVENT_Set_Position:
		{
			if (unit_has_control(unit))
				break;

			Vec2 position;
			channel_read(chnl, &position);
			unit->net_position = position;

#if SERVER
			if (src != nullptr)
				channel_rebroadcast_last(chnl, false);
#endif
			break;
		}

		case EVENT_Shoot:
		{
			Vec2 origin;
			Vec2 direction;
			channel_read(chnl, &origin);
			channel_read(chnl, &direction);

			scene_make_projectile(scene_unit_handle(unit), 0, origin + direction * 1.6f, direction);

#if CLIENT
			unit->gun_billboard->position -= Vec3(direction, 0.f) * 0.3f;
#endif

#if SERVER
			if (src != nullptr)
				channel_rebroadcast_last(chnl, true);
#endif
			break;
		}

		case EVENT_Hit:
		{
			// Read and retaliate towards the one who shot us
			i32 source_id;
			channel_read(chnl, &source_id);

#if SERVER
			if (source_id != -1)
				unit->target = scene_unit_handle(source_id);
#endif

			// Add impulse
			Vec2 impulse;
			channel_read(chnl, &impulse);

			unit->impact_velocity = impulse;

#if CLIENT
			unit->hit_timer = unit_hit_duration;
#endif

#if SERVER
			if (src != nullptr)
				channel_rebroadcast_last(chnl, true);
#endif

			// Subtract health and die
			unit->health -= 1.f;
			if (unit->health <= 0.f)
				scene_destroy_unit(unit);

			break;
		}

		case EVENT_Set_Target:
		{
			u32 id;
			channel_read(chnl, &id);
			unit->target = scene_unit_handle(id);

			break;
		}

		case EVENT_Set_Aim_Direction:
		{
			if (unit_has_control(unit))
				return;

			Vec2 direction;
			channel_read(chnl, &direction);

			unit->aim_direction = direction;
			break;
		}

		case EVENT_Die:
		{
			break;
		}
	}
}

void unit_init(Unit* unit, u32 id, const Vec2& position)
{
	unit->id = id;
	unit->position = position;
	unit->net_position = position;

	unit->channel = channel_open("UNIT", id, unit_event_proc);
	unit->channel->user_ptr = unit;

	unit->health = unit->health_max;
	unit->position_sync_timer.interval = 1.f / unit_sync_frequency;

#if CLIENT

	unit->billboard = scene_make_billboard(sprite_sheet_load("Sprite/unit_sheet.dat"));
	unit->billboard->position = Vec3(position, 0.f);
	unit->billboard->anchor = Vec2(0.5f, 1.f);

	unit->gun_billboard = scene_make_billboard(sprite_sheet_load("Sprite/weapon_sheet.dat"));
	unit->gun_billboard->position = Vec3(position, 0.f);
	unit->gun_billboard->anchor = Vec2(0.5f, 0.5f);

	unit->health_bar = scene_make_health_bar();
	unit->health_bar->position = Vec3(position, 2.f);

#elif SERVER

	unit->ai_walk_target = position;
	unit->ai_shoot_timer = random_float(2.f, 8.f);

#endif
}

void unit_free(Unit* unit)
{
	channel_close(unit->channel);

#if CLIENT
	scene_destroy_billboard(unit->billboard);
	scene_destroy_billboard(unit->gun_billboard);
	scene_destroy_health_bar(unit->health_bar);
#endif
}

void unit_update(Unit* unit)
{
	// Apply impact velocity
	unit->position += unit->impact_velocity * time_delta();
	if (!unit_has_control(unit))
		unit->net_position += unit->impact_velocity * time_delta();
	unit->impact_velocity -= unit->impact_velocity * unit_impact_drag * time_delta();

#if CLIENT

	// Unit billboard
	unit->billboard->position = Vec3(unit->position, 0.f);

	// Gun billboard
	Vec3 gun_target = Vec3(unit->position, 0.f) + Vec3(unit->aim_direction * 0.6f, 0.5f);
	unit->gun_billboard->position = lerp(unit->gun_billboard->position, gun_target, 21.f * time_delta());

	// Calculate the gun rotation by getting the angle in screen-space from the unit to the 
	//	aim target, and setting that as the angle
	Vec2 unit_screen = scene_project_to_screen(Vec3(unit->position, 0.f));
	Vec2 gun_screen = scene_project_to_screen(Vec3(unit->position + unit->aim_direction, 0.f));
	Vec2 screen_direction = normalize_safe(gun_screen - unit_screen);

	// Lerp it 
	float target_angle = atan2(screen_direction.y, screen_direction.x);
	unit->gun_billboard->rotation = lerp_radians(unit->gun_billboard->rotation, target_angle, 12.f * time_delta());

	// Flip the gun if its aiming to the left, since it would be upside down
	if (target_angle > HALF_PI || target_angle < -HALF_PI)
		unit->gun_billboard->scale.y = -1.f;
	else
		unit->gun_billboard->scale.y = 1.f;

	// Update health bar
	unit->health_bar->position = Vec3(unit->position, 2.f);
	unit->health_bar->health_percent = unit->health / unit->health_max;

	// When hit, flash for a bit!
	unit->hit_timer -= time_delta();
	unit->billboard->fill_color = Vec4(Vec3(1.f), unit->hit_timer > 0.f ? 1.f : 0.f);
	unit->billboard->scale = Vec2(unit->hit_timer > 0.f ? 1.f + unit->hit_timer : 1.f);

#elif SERVER

	// AI movement
	if (unit->owner == nullptr)
	{
		unit->walk_timer -= time_delta();
		if (unit->walk_timer <= 0.f)
		{
			unit_move_towards(unit, unit->ai_walk_target);
			if (distance_sqrd(unit->position, unit->ai_walk_target) < 0.2f)
			{
				unit->walk_timer = random_float(2.f, 4.f);
				unit->ai_walk_target = Vec2(random_float(-5.f, 5.f), random_float(-5.f, 5.f));
			}
		}

		Unit* target = scene_get_unit(unit->target);
		if (target)
		{
			Vec2 direction = normalize_safe(target->position - unit->position);
			unit->aim_direction = direction;

			unit->ai_shoot_timer -= time_delta();
			if (unit->ai_shoot_timer <= 0.f)
			{
				unit_shoot(unit, target->position);
				unit->ai_shoot_timer = random_float(1.f, 2.f);
			}

			channel_reset(unit->channel);
			channel_write_u8(unit->channel, EVENT_Set_Aim_Direction);
			channel_write_vec2(unit->channel, direction);
			channel_broadcast(unit->channel, false);
		}
	}

#endif

	// Sync position!
	if (unit_has_control(unit))
	{
		if (timer_update(&unit->position_sync_timer))
		{
			channel_reset(unit->channel);
			channel_write_u8(unit->channel, EVENT_Set_Position);
			channel_write_vec2(unit->channel, unit->position);
			channel_broadcast(unit->channel, false);
		}
	}
	else
	{
		unit_move_towards(unit, unit->net_position);
	}
}

#if SERVER
void unit_sync_to(Unit* unit, Online_User* user)
{
}
#endif

void unit_serialize_to(Unit* unit, Online_User* user)
{
	if (scene_get_unit(unit->target))
	{
		channel_reset(unit->channel);
		channel_write_u8(unit->channel, EVENT_Set_Target);
		channel_write_u32(unit->channel, unit->target.index);
		channel_send(unit->channel, user, true);
	}
}

void unit_move_towards(Unit* unit, const Vec2& target)
{
	unit_move_direction(unit, target - unit->position);
}

void unit_move_direction(Unit* unit, const Vec2& direction)
{
	if (nearly_zero(direction))
		return;

	Vec2 dir_norm = normalize(direction);
	unit->position += dir_norm * unit->move_speed * time_delta();
}

void unit_shoot(Unit* unit, const Vec2& target)
{
	Vec2 direction = normalize(target - unit->position);

	channel_reset(unit->channel);
	channel_write_u8(unit->channel, EVENT_Shoot);
	channel_write_vec2(unit->channel, unit->position);
	channel_write_vec2(unit->channel, direction);
	channel_broadcast(unit->channel, false);
}

void unit_hit(Unit* unit, const Unit_Handle& source, const Vec2& impulse)
{
	Unit* source_unit = scene_get_unit(source);

	channel_reset(unit->channel);
	channel_write_u8(unit->channel, EVENT_Hit);

	// Write ID of source unit if there is one, otherwise write -1
	if (source_unit)
		channel_write_i32(unit->channel, source_unit->id);
	else
		channel_write_i32(unit->channel, -1);

	channel_write_vec2(unit->channel, impulse);
	channel_broadcast(unit->channel, true);
}

bool unit_has_control(Unit* unit)
{
#if SERVER
	return unit->owner == nullptr;
#elif CLIENT
	return unit->is_local;
#endif
}