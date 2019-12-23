#include "Unit.h"
#include "Engine/Render/Billboard.h"
#include "Runtime/Game/Scene.h"
#include "Runtime/Online/Channel.h"

enum Unit_Event
{
	EVENT_Set_Position,
	EVENT_Shoot,
	EVENT_Hit,
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
			unit->position = position;

#if SERVER
			if (src != nullptr)
			{
				channel_reset(chnl);
				channel_write_except(chnl, src);
				channel_write_u8(chnl, EVENT_Set_Position);
				channel_write_vec2(chnl, position);
				channel_broadcast(chnl, false);
			}
#endif
			break;
		}

		case EVENT_Shoot:
		{
			Vec2 origin;
			Vec2 direction;
			channel_read(chnl, &origin);
			channel_read(chnl, &direction);

			projectile_spawn(unit, 0, origin + direction * 1.6f, direction);

#if CLIENT
			unit->gun_billboard->position -= Vec3(direction, 0.f) * 0.3f;
#endif

#if SERVER
			if (src != nullptr)
			{
				channel_reset(chnl);
				channel_write_except(chnl, src);
				channel_write_u8(chnl, EVENT_Shoot);
				channel_write_vec2(chnl, origin);
				channel_write_vec2(chnl, direction);
				channel_broadcast(chnl, false);
			}
#endif
			break;
		}

		case EVENT_Hit:
		{
#if SERVER
			Vec2 impulse;
			channel_read(chnl, &impulse);

			unit->position += impulse;
#endif

			break;
		}
	}
}

Unit* unit_spawn(u32 id, const Vec2& position)
{
	Unit* unit = &scene.units[id];
	assert_msg(!unit->active, "Tried to spawn unit %d, but it's already active");

	unit->active = true;
	unit->id = id;
	unit->position = position;
	unit->target_position = position;

	unit->channel = channel_open("UNIT", id, unit_event_proc);
	unit->channel->user_ptr = unit;

#if CLIENT
	unit->billboard = billboard_load("Sprite/unit_sheet.dat");
	unit->billboard->position = Vec3(position, 0.f);
	unit->billboard->anchor = Vec2(0.5f, 1.f);

	unit->gun_billboard = billboard_load("Sprite/weapon_sheet.dat");
	unit->gun_billboard->position = Vec3(position, 0.f);
	unit->gun_billboard->anchor = Vec2(0.5f, 0.5f);
#elif SERVER
	unit->ai_walk_target = position;
#endif

	return unit;
}

void unit_destroy(Unit* unit)
{
	assert_msg(unit->active, "Tried to destroy %d, but it's not active", unit->id);

	channel_close(unit->channel);

#if CLIENT
	billboard_destroy(unit->billboard);
	unit->billboard = nullptr;
#endif

	*unit = Unit();
}

inline void unit_update(Unit* unit)
{
#if CLIENT
	unit->billboard->position = Vec3(unit->position, 0.f);

	Vec3 gun_target = Vec3(unit->position, 0.f) + Vec3(unit->aim_direction * 0.6f, 0.5f);
	unit->gun_billboard->position = lerp(unit->gun_billboard->position, gun_target, 21.f * time_delta());

	Vec2 unit_screen = scene_project_to_screen(Vec3(unit->position, 0.f));
	Vec2 gun_screen = scene_project_to_screen(Vec3(unit->position + unit->aim_direction, 0.f));
	Vec2 screen_direction = gun_screen - unit_screen;

	float target_angle = atan2(screen_direction.y, screen_direction.x);
	unit->gun_billboard->rotation = lerp_radians(unit->gun_billboard->rotation, target_angle, 12.f * time_delta());
	if (target_angle > HALF_PI || target_angle < -HALF_PI)
		unit->gun_billboard->scale.y = -1.f;
	else
		unit->gun_billboard->scale.y = 1.f;

#elif SERVER
	if (unit->owner == nullptr)
	{
		unit_move_towards(unit, unit->ai_walk_target);
	}
#endif
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

	channel_reset(unit->channel);
	channel_write_u8(unit->channel, EVENT_Set_Position);
	channel_write_vec2(unit->channel, unit->position);
	channel_broadcast(unit->channel, false);
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

void unit_hit(Unit* unit, const Vec2& impulse)
{
	channel_reset(unit->channel);
	channel_write_u8(unit->channel, EVENT_Hit);
	channel_write_vec2(unit->channel, impulse);
	channel_broadcast(unit->channel, true);
}

void units_update()
{
	for(u32 i=0; i<MAX_UNITS; ++i)
	{
		if (!scene.units[i].active)
			continue;

		unit_update(scene.units + i);
	}
}

bool unit_has_control(Unit* unit)
{
#if SERVER
	return unit->owner == nullptr;
#elif CLIENT
	return unit->is_local;
#endif
}