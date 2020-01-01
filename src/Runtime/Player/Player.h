#pragma once
#include "Core/Time/IntervalTimer.h"
#include "Runtime/Game/HandleTypes.h"
#include "Runtime/Online/Channel.h"

struct Unit;
struct Online_User;

const float player_force_sync_frequency = 10.f;
const float player_net_correction_coefficient = 10.f;

struct Player
{
	u32 id;

#if CLIENT
	bool is_local = false;
	Interval_Timer force_sync_timer;
#endif

	Unit_Handle controlled_unit;
	Channel* channel;

	Vec2 net_position_error;
	Vec2 move_input;
	Vec2 aim_position;
};

void player_init(Player* player, u32 id, const Unit_Handle& unit_to_control);
void player_update(Player* player);