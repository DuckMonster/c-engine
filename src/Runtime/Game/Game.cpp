#include "Game.h"
#include "Engine/Render/Render.h"
#include "Runtime/Game/Scene.h"
#include "Runtime/Online/Online.h"
#include "Runtime/Unit/Unit.h"

Game game;

void game_event_proc(Channel* chnl)
{
	u8 event_id;
	channel_read(chnl, &event_id);

	switch(event_id)
	{
		case 0:
		{
			u32 player_id;
			channel_read(chnl, &player_id);
			debug_log("Player spawned! ID: %d", player_id);
			break;
		}

		case 1:
		{
			u32 unit_id;
			channel_read(chnl, &unit_id);
			debug_log("Possess unit ID %d", unit_id);
			break;
		}

		default:
			debug_log("Received unknown event ID %d", event_id);
			break;
	}
}

#if SERVER
void server_spawn_unit()
{
}

void server_destroy_unit()
{
}
#endif

void game_init()
{
	scene_init();

	game.channel = channel_open("GAME", 0, game_event_proc);

	#if SERVER
	u32 num = random_int(2, 5);
	for(u32 i=0; i<num; ++i)
	{
		server_spawn_unit();
	}
	#endif
}

#if SERVER
void game_user_added(Online_User* user)
{
	channel_reset(game.channel);
	channel_write_u8(game.channel, 0);
	channel_write_u32(game.channel, user->id);
	channel_broadcast(game.channel, true);

	channel_reset(game.channel);
	channel_write_u8(game.channel, 1);
	channel_write_u32(game.channel, user->id);
	channel_send(game.channel, user, true);
}

void game_user_leave(Online_User* user)
{
}
#endif

void game_update()
{
#if CLIENT
	camera_update(&scene.camera);
	render_set_vp(camera_view_matrix(&scene.camera), camera_projection_matrix(&scene.camera));
#endif

#if SERVER
	static float spawn_timer = 0.f;
	static bool destroy = false;

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
