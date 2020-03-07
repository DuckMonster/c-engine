#pragma once
struct Player;
struct Item_Drop;

const float player_item_hover_radius = 0.8f;
const float player_aim_lerp_speed = 12.f;

struct Player_Shooting
{
	Vec3 aim_position;
	Vec3 aim_position_remote;
	Item_Drop* hovered_drop = nullptr;

	Interval_Timer aim_sync_timer;
};

void player_shooting_update(Player* player);