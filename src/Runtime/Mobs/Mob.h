#pragma once
#include "Core/Time/IntervalTimer.h"
#include "Runtime/Game/HandleTypes.h"
#include "Runtime/Online/Channel.h"

struct Unit;

const float mob_shoot_range = 8.f;
const float mob_aim_duration = 0.4f;
const float mob_predict_lerp_speed = 10.f;

struct Mob
{
	u32 id;
	Unit_Handle controlled_unit;

	Channel* channel;
	Vec3 target_position;

	Interval_Timer idle_timer;

	Unit_Handle agroo_target;
	Interval_Timer shoot_timer;

	Vec3 aim_target;
	float aim_timer = 0.f;
	bool is_aiming = false;

	Vec3 target_predict_velocity;
};

void mob_init(Mob* mob, u32 id, const Unit_Handle& unit_to_control);
void mob_free(Mob* mob);

#if SERVER
void mob_set_agroo(Mob* mob, Unit* unit);
void mob_update_target_position(Mob* mob, const Vec3& center, float radius_min, float radius_max);
#endif

void mob_update(Mob* mob);