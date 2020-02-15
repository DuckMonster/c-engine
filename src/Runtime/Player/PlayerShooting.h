#pragma once
struct Player;
struct Item_Drop;

struct Player_Shooting
{
	Vec2 aim_position;
	Item_Drop* hovered_drop = nullptr;
};

void player_shooting_update(Player* player);