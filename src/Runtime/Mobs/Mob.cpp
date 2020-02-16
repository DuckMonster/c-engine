#include "Mob.h"
#include "Runtime/Unit/Unit.h"
#include "Runtime/Game/Scene.h"
#include "Runtime/Game/SceneQuery.h"
#include "Runtime/Game/Game.h"

enum Mob_Events
{
	EVENT_Set_Target,
	EVENT_Set_Agroo,
};

void mob_event_proc(Channel* chnl, Online_User* src)
{
	Mob* mob = (Mob*)chnl->user_ptr;

	u8 event_type;
	channel_read(chnl, &event_type);

	switch(event_type)
	{
		case EVENT_Set_Target:
		{
			Vec3 target;
			channel_read(chnl, &target);

			mob->target_position = target;
			break;
		}

		case EVENT_Set_Agroo:
		{
			i32 id;
			channel_read(chnl, &id);

			if (id < 0)
				mob->agroo_target = Unit_Handle();
			else
				mob->agroo_target = scene_unit_handle(id);
			break;
		}
	}
}

void mob_init(Mob* mob, u32 id, const Unit_Handle& unit_to_control)
{
	Unit* unit = scene_get_unit(unit_to_control);

	mob->id = id;
	mob->controlled_unit = unit_to_control;
	mob->channel = channel_open("MOB", id, mob_event_proc);
	mob->channel->user_ptr = mob;

	if (unit)
	{
		mob->target_position = unit->position;
		unit->mob_owner = game_mob_handle(mob);
	}

	mob->idle_timer.interval = 5.f;
	mob->idle_timer.variance = 2.f;
	mob->shoot_timer.interval = 2.f;
	mob->shoot_timer.variance = 1.f;
}

void mob_free(Mob* mob)
{
	channel_close(mob->channel);

	// If the unit is still alive, tell them we're not controlling it anymore
	Unit* unit = scene_get_unit(mob->controlled_unit);
	if (unit)
		unit->mob_owner = Mob_Handle();
}

#if SERVER
void mob_set_agroo(Mob* mob, Unit* unit)
{
	channel_reset(mob->channel);
	channel_write_u8(mob->channel, EVENT_Set_Agroo);
	if (unit)
		channel_write_i32(mob->channel, unit->id);
	else
		channel_write_i32(mob->channel, -1);
	channel_broadcast(mob->channel, true);
}

void mob_update_target_position(Mob* mob, const Vec3& center, float radius_min, float radius_max)
{
	Unit* unit = scene_get_unit(mob->controlled_unit);
	if (!unit)
		return;

	/* We only want to walk towards visible locations, unless enough fails in which case fuck it */
	Vec3 target_position;
	int tries = 5;
	while(tries--)
	{
		target_position = center + Vec3(random_point_on_circle(), 0.f) * random_float(radius_min, radius_max);
		if (scene_query_vision(unit->position, target_position))
			break;
	}

	channel_reset(mob->channel);
	channel_write_u8(mob->channel, EVENT_Set_Target);
	channel_write_vec3(mob->channel, target_position);
	channel_broadcast(mob->channel, true);
}
#endif

void mob_update(Mob* mob)
{
	Unit* unit = scene_get_unit(mob->controlled_unit);
	if (!unit)
	{
#if SERVER
		game_destroy_mob(mob);
#endif
		return;
	}

	unit_move_towards(unit, mob->target_position);

	Unit* target_unit = scene_get_unit(mob->agroo_target);
	if (target_unit)
	{
		// Check shoot range
		float distance_to_target_sqrd = distance_sqrd(unit->position, target_unit->position);
		bool is_within_range = distance_to_target_sqrd < square(mob_shoot_range);

#if SERVER 
		// Check vision
		bool target_is_visible = scene_query_vision(unit->position, target_unit->position);
		if (is_within_range && target_is_visible)
		{
			// Shoot!
			//if (timer_update(&mob->shoot_timer))
				//unit_shoot(unit, target_unit->position);
		}
		else
		{
			// Chase!
			if (distance_sqrd(unit->position, mob->target_position) < square(1.f))
				mob_update_target_position(mob, target_unit->position, mob_shoot_range * 0.3f, mob_shoot_range * 0.9f);
		}
#endif
	}

#if SERVER
	if (timer_update(&mob->idle_timer))
	{
		mob_update_target_position(mob, unit->position, 1.f, 5.f);
	}
#endif

	Unit* shoot_target_unit = scene_get_unit(mob->agroo_target);
	if (shoot_target_unit)
	{
		unit->aim_direction = normalize(shoot_target_unit->position - unit->position);
	}
}