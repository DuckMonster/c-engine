#pragma once
#include "Core/Time/IntervalTimer.h"
#include "Runtime/Game/HandleTypes.h"
#include "Runtime/Online/Channel.h"

struct Mob
{
	u32 id;
	Unit_Handle controlled_unit;

	Channel* channel;
	Vec2 target_position;

	Interval_Timer idle_timer;

	Unit_Handle agroo_target;
	Interval_Timer shoot_timer;
};

void mob_init(Mob* mob, u32 id, const Unit_Handle& unit_to_control);
void mob_free(Mob* mob);
#if SERVER
void mob_set_agroo(Mob* mob, Unit* unit);
#endif
void mob_update(Mob* mob);