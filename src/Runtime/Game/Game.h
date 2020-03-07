#pragma once
#include "Core/Time/IntervalTimer.h"
#include "Engine/Collision/CollisionTypes.h"
#include "Runtime/Game/HandleTypes.h"
#include "Runtime/Game/Camera.h"
#include "Runtime/Editor/Editor.h"

struct Channel;
struct Online_User;
struct Drawable;
struct Weapon_Instance;

struct Player;
struct Mob;

#define MAX_PLAYERS 10
#define MAX_MOBS 40

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
#endif

#if SERVER
	Interval_Timer ai_spawn_timer;
#endif
};
extern Game game;

void game_init();
void game_update();
#if CLIENT
void game_render(const Render_State& state);
#endif

Player* game_get_player(const Player_Handle& player_hndl);
Player_Handle game_player_handle(Player* player);
Mob* game_get_mob(const Mob_Handle& mob_hndl);
Mob_Handle game_mob_handle(Mob* mob);

#if CLIENT

Mat4 game_ndc_to_pixel();
Ray game_mouse_ray();
Ray game_screen_to_ray(Vec2 screen);
Vec2 game_project_to_screen(const Vec3& position);

#endif

#if SERVER

Unit* game_spawn_random_unit();
Unit* game_spawn_unit_at(const Vec3& position);
void game_destroy_unit(Unit* unit);

void game_user_added(Online_User* user);
void game_user_leave(Online_User* user);
Mob* game_create_mob_for_unit(Unit* unit);
void game_destroy_mob(Mob* mob);

void game_create_item_drop(const Vec3& position, const Weapon_Instance& weapon);

#endif