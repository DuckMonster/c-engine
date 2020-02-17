#pragma once
struct Player;
struct Item_Drop;

const float player_item_hover_radius = 0.8f;

struct Player_Shooting
{
	Vec3 aim_position;
	Item_Drop* hovered_drop = nullptr;
};

void player_shooting_update(Player* player);