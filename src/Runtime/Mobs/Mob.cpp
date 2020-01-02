#include "Mob.h"
#include "Runtime/Game/Scene.h"
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
			Vec2 target;
			channel_read(chnl, &target);

			mob->target_position = target;
			break;
		}

		case EVENT_Set_Agroo:
		{
			u32 id;
			channel_read(chnl, &id);

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

#if SERVER
void mob_set_agroo(Mob* mob, Unit* unit)
{
	channel_reset(mob->channel);
	channel_write_u8(mob->channel, EVENT_Set_Agroo);
	channel_write_u32(mob->channel, unit->id);
	channel_broadcast(mob->channel, true);
}
#endif

void mob_update(Mob* mob)
{
	Unit* unit = scene_get_unit(mob->controlled_unit);
	if (!unit)
		return;

	unit_move_towards(unit, mob->target_position);

#if SERVER
	if (timer_update(&mob->idle_timer))
	{
		channel_reset(mob->channel);
		channel_write_u8(mob->channel, EVENT_Set_Target);
		channel_write_vec2(mob->channel, random_point_on_circle() * random_float(0.f, 5.f));
		channel_broadcast(mob->channel, true);
	}
#endif

	Unit* shoot_target_unit = scene_get_unit(mob->agroo_target);
	if (shoot_target_unit)
	{
		unit->aim_direction = normalize(shoot_target_unit->position - unit->position);

#if SERVER
		if (timer_update(&mob->shoot_timer))
		{
			unit_shoot(unit, shoot_target_unit->position);
		}
#endif
	}
}