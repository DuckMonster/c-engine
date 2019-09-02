#pragma once
struct Billboard;

#if CLIENT
struct Player
{
	Vec3 position;
	Billboard* billboard;
};

void player_create(Player* player);
void player_update(Player* player);
#endif