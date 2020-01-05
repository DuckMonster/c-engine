#pragma once
#include "Core/Time/IntervalTimer.h"
#include "Engine/Collision/CollisionTypes.h"
#include "Runtime/Player/Player.h"
#include "Runtime/Mobs/Mob.h"
#include "Runtime/Game/HandleTypes.h"
#include "Runtime/Game/Camera.h"
#include "Runtime/Editor/Editor.h"

struct Channel;
struct Online_User;
struct Drawable;

#define MAX_PLAYERS 10
#define MAX_MOBS 5

struct Game
{
	Thing_Array<Player> players;
	Thing_Array<Mob> mobs;

	Channel* channel;

#if CLIENT
	Player* local_player = nullptr;
	u32 tile_size;

	Drawable* floor;
	Camera camera;

	bool is_editor = false;
	Editor editor;
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

#if CLIENT
Ray game_mouse_ray();
Ray game_screen_to_ray(Vec2 screen);
Vec2 game_project_to_screen(const Vec3& position);
#endif

#if SERVER

Unit* game_spawn_random_unit();
void game_destroy_unit(Unit* unit);

void game_user_added(Online_User* user);
void game_user_leave(Online_User* user);
void game_create_mob_for_unit(Unit* unit);
void game_destroy_mob(Mob* mob);

#endif