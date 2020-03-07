#pragma once
#include "Core/Time/IntervalTimer.h"
#include "Runtime/Game/HandleTypes.h"
#include "Runtime/Online/Channel.h"
#include "PlayerMovement.h"
#include "PlayerShooting.h"

struct Unit;
struct Online_User;
struct Item_Drop;

enum Player_Event
{
	PLAYEREV_Set_Input,
	PLAYEREV_Set_Aim_Position,
	PLAYEREV_Pickup_Weapon,
};

struct Player
{
	u32 id;

#if CLIENT
	bool is_local = false;
#endif

	Unit_Handle controlled_unit;
	Channel* channel;

	// Movement stuff
	Player_Movement movement;
	Player_Shooting shooting;
};

void player_init(Player* player, u32 id, const Unit_Handle& unit_to_control);
void player_free(Player* player);
void player_update(Player* player);