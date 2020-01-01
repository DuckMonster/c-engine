#pragma once
#include "Runtime/Player/Player.h"
#include "Runtime/Game/HandleTypes.h"
#include "Core/Time/IntervalTimer.h"

struct Channel;
struct Online_User;

#define MAX_PLAYERS 10

struct Game
{
	Thing_Array<Player> players;

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

#if SERVER
void game_user_added(Online_User* user);
void game_user_leave(Online_User* user);
#endif SERVER