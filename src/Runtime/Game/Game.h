#pragma once
#include "Runtime/Player/Player.h"
#include "Runtime/Mobs/Mob.h"
#include "Runtime/Game/HandleTypes.h"
#include "Core/Time/IntervalTimer.h"

struct Channel;
struct Online_User;

#define MAX_PLAYERS 10
#define MAX_MOBS 50

struct Game
{
	Thing_Array<Player> players;
	Thing_Array<Mob> mobs;

	Channel* channel;

#if CLIENT
	Player* local_player = nullptr;
	u32 tile_size;
#endif

#if SERVER
	Interval_Timer ai_spawn_timer;
#endif
};
extern Game game;

void game_init();
void game_update();

Player* game_get_player(const Player_Handle& player_hndl);
Player_Handle game_player_handle(Player* player);
Mob* game_get_mob(const Mob_Handle& mob_hndl);
Mob_Handle game_mob_handle(Mob* mob);

#if SERVER
void game_user_added(Online_User* user);
void game_user_leave(Online_User* user);
void game_create_mob_for_unit(Unit* unit);
#endif SERVER