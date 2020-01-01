#include "Game.h"
#include "Engine/Config/Config.h"
#include "Runtime/Render/Render.h"
#include "Runtime/Game/Scene.h"
#include "Runtime/Online/Online.h"
#include "Runtime/Unit/Unit.h"

Game game;

enum Game_Event
{
	EVENT_Unit_Spawn,
	EVENT_Unit_Destroy,
	EVENT_Player_Possess,
};

void game_event_proc(Channel* chnl, Online_User* src)
{
	u8 event_type;
	channel_read(chnl, &event_type);

	switch(event_type)
	{
		case EVENT_Unit_Spawn:
		{
			u32 unit_id;
			Vec2 unit_position;
			channel_read(chnl, &unit_id);
			channel_read(chnl, &unit_position);

			scene_make_unit(unit_id, unit_position);
			break;
		}

		case EVENT_Unit_Destroy:
		{
			u32 unit_id;
			channel_read(chnl, &unit_id);

			Unit* unit = scene.units[unit_id];
			scene_destroy_unit(unit);
			break;
		}

		case EVENT_Player_Possess:
		{
#if CLIENT
			u32 unit_id;
			channel_read(chnl, &unit_id);
			assert_msg(scene.units[unit_id], "Received possess %d, but that unit is not active", unit_id);

			game.local_unit = scene_unit_handle(unit_id);
			scene.units[unit_id]->is_local = true;
#endif
			break;
		}

		default:
			debug_log("Received unknown event type %d", event_type);
			break;
	}
}

#if SERVER
Unit* server_spawn_unit()
{
	u32 unit_id = scene_get_free_unit_id();

	Vec2 spawn_position;
	spawn_position.x = random_float(-5.f, 5.f);
	spawn_position.y = random_float(-5.f, 5.f);

	channel_reset(game.channel);
	channel_write_u8(game.channel, EVENT_Unit_Spawn);
	channel_write_u32(game.channel, unit_id);
	channel_write_vec2(game.channel, spawn_position);
	channel_broadcast(game.channel, true);

	return scene.units[unit_id];
}

void server_destroy_unit(u32 unit_id)
{
	channel_reset(game.channel);
	channel_write_u8(game.channel, EVENT_Unit_Destroy);
	channel_write_u32(game.channel, unit_id);
	channel_broadcast(game.channel, true);
}
#endif

void game_init()
{
	scene_init();

	game.channel = channel_open("GAME", 0, game_event_proc);

#if CLIENT
	game.tile_size = 24;
	config_get("game.tile_size", &game.tile_size);
#endif

#if SERVER
	u32 num = random_int(2, 10);
	for(u32 i=0; i<num; ++i)
	{
		server_spawn_unit();
	}

	game.ai_spawn_timer.interval = 10.f;
	game.ai_spawn_timer.variance = 5.f;
#endif
}

#if SERVER
void game_user_added(Online_User* user)
{
	THINGS_FOREACH(&scene.units)
	{
		Unit* unit = it;

		channel_reset(game.channel);
		channel_write_u8(game.channel, EVENT_Unit_Spawn);
		channel_write_u32(game.channel, unit->id);
		channel_write_vec2(game.channel, unit->position);
		channel_send(game.channel, user, true);
	}

	Unit* player_unit = server_spawn_unit();

	player_unit->owner = user;
	channel_reset(game.channel);
	channel_write_u8(game.channel, EVENT_Player_Possess);
	channel_write_u32(game.channel, player_unit->id);
	channel_send(game.channel, user, true);
}

void game_user_leave(Online_User* user)
{
	Unit* owned_unit = nullptr;
	THINGS_FOREACH(&scene.units)
	{
		if (it->owner == user)
		{
			owned_unit = it;
			break;
		}
	}

	if (owned_unit)
		server_destroy_unit(owned_unit->id);
}
#endif

void game_update()
{
#if CLIENT

	Unit* local_unit = scene_get_unit(game.local_unit);
	if (local_unit)
		player_control(local_unit);

	camera_update(&scene.camera);
	render_set_vp(camera_view_matrix(&scene.camera), camera_projection_matrix(&scene.camera));

#elif SERVER

	if (timer_update(&game.ai_spawn_timer))
	{
		server_spawn_unit();
	}

#endif

	scene_update();
}