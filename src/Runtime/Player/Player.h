#pragma once
#include "Core/Time/IntervalTimer.h"
#include "Runtime/Game/HandleTypes.h"
#include "Runtime/Online/Channel.h"
#include "PlayerMovement.h"
#include "PlayerShooting.h"

struct Unit;
struct Online_User;
struct Item_Drop;

const float player_force_sync_frequency = 10.f;
const float player_net_correction_coefficient = 10.f;
const float player_item_hover_radius = 0.8f;

enum Player_Event
{
	PLAYEREV_Set_Input,
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