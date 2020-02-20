#pragma once
#include "Core/Time/IntervalTimer.h"

struct Player;
struct Channel;
struct Online_User;

const float player_gravity = 30.f;
const float player_acceleration = 180.f;
const float player_drag = 18.f;
const float player_force_sync_frequency = 10.f;
const float player_net_correction_coefficient = 10.f;
const float player_dash_hori_impulse = 15.f;
const float player_dash_vert_impulse = 5.f;
const float player_dash_drag = 2.1f;

struct Player_Movement
{
#if CLIENT
	Interval_Timer force_sync_timer;
#endif

	Vec3 net_position_error;
	Vec2 move_input;

	bool is_dashing = false;
	Vec3 dash_velocity;
	float dash_influence = 0.5f;
};

void player_movement_init(Player* player);
void player_movement_update(Player* player);
void player_movement_recv_remote_input(Player* player, const Vec2& remote_input, const Vec3& remote_position);