#pragma once
#include "Core/Time/IntervalTimer.h"

struct Player;
struct Channel;
struct Online_User;

struct Player_Movement
{
#if CLIENT
	Interval_Timer force_sync_timer;
#endif

	Vec2 net_position_error;
	Vec2 move_input;
};

void player_movement_init(Player* player);
void player_movement_update(Player* player);
void player_movement_recv_remote_input(Player* player, const Vec2& remote_input, const Vec2& remote_position);