#include "Game.h"
#include "Engine/Render/Render.h"
#include "Runtime/Game/Scene.h"
#include "Runtime/Online/Online.h"
#include "Runtime/Unit/Unit.h"

Game game;

namespace
{
	enum Events
	{
		EVENT_Spawn_Unit,
		EVENT_Destroy_Unit,
	};

	struct Spawn_Unit_Data
	{
		u32 id;
		Vec2 position;
	};

	struct Destroy_Unit_Data
	{
		u32 id;
	};

	void ev_spawn_unit(Game* game, Spawn_Unit_Data* data)
	{
		unit_spawn(data->id, data->position);
	}

	void ev_destroy_unit(Game* game, Destroy_Unit_Data* data)
	{
		unit_destroy(scene.units + data->id);
	}
}

#if SERVER
void server_spawn_unit()
{
	Vec2 position = Vec2(random_float(-5.f, 5.f), random_float(-5.f, 5.f));

	Spawn_Unit_Data spawn;
	spawn.id = scene_get_free_unit();
	spawn.position = position;

	channel_post_t(game.channel, EVENT_Spawn_Unit, spawn);
}

void server_destroy_unit()
{
	u32 random_id = 0;
	do
	{
		random_id = random_int(0, MAX_UNITS);
	} while(!scene.units[random_id].active);

	Destroy_Unit_Data data;
	data.id = random_id;

	channel_post_t(game.channel, EVENT_Destroy_Unit, data);
}
#endif

void game_init()
{
	scene_init();

	game.channel = channel_open("GAME", 0, &game);
	channel_register_event(game.channel, EVENT_Spawn_Unit, EVENT_Broadcast, true, ev_spawn_unit);
	channel_register_event(game.channel, EVENT_Destroy_Unit, EVENT_Broadcast, true, ev_destroy_unit);

#if SERVER
	u32 num = random_int(2, 5);
	for(u32 i=0; i<num; ++i)
	{
		server_spawn_unit();
	}
#endif
}

void game_user_added(Online_User* user)
{
	Spawn_Unit_Data spawn;

	// Send all units to the new user
	for(u32 i=0; i<MAX_UNITS; ++i)
	{
		if (scene.units[i].active)
		{
			spawn.id = i;
			spawn.position = scene.units[i].position;

			channel_post_to_t(game.channel, user, EVENT_Spawn_Unit, spawn);
		}
	}
}

void game_user_leave(Online_User* user)
{
}

void game_update()
{
#if CLIENT
	camera_update(&scene.camera);
	render_set_vp(camera_view_matrix(&scene.camera), camera_projection_matrix(&scene.camera));
#endif

#if SERVER
	static float spawn_timer = 0.f;
	static bool destroy = false;

	spawn_timer += time_delta();
	if (spawn_timer > 2.f)
	{
		if (destroy)
			server_destroy_unit();
		else
			server_spawn_unit();

		spawn_timer = 0.f;
		destroy = !destroy;
	}

	for(u32 i=0; i<MAX_UNITS; ++i)
	{
		if (!scene.units[i].active)
			continue;

		scene.units[i].walk_timer -= time_delta();

		if (scene.units[i].walk_timer > 0.f)
			continue;

		Vec2 target;
		target.x = random_float(-10.f, 10.f);
		target.y = random_float(-10.f, 10.f);
		unit_move_to(scene.units + i, target);

		scene.units[i].walk_timer = random_float(3.f, 6.f);
	}
#endif

	units_update();
}