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
	EVENT_Player_Join,
	EVENT_Player_Leave,
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

		case EVENT_Player_Join:
		{
			u32 player_id;
			i32 unit_id;
			channel_read(chnl, &player_id);
			channel_read(chnl, &unit_id);

			Unit_Handle unit_hndl;
			// If unit_id < 0, then the player doesn't control a unit
			if (unit_id >= 0)
				unit_hndl = scene_unit_handle(unit_id);

			Player* player = thing_add_at(&game.players, player_id);
			player_init(player, player_id, unit_hndl);

			debug_log("Creating player %d", player_id);

#if CLIENT
			if (client_is_self(player_id))
				game.local_player = player;
#endif

			break;
		}

		case EVENT_Player_Leave:
		{
			u32 player_id;
			channel_read(chnl, &player_id);

			thing_remove_at(&game.players, player_id);

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
	thing_array_init(&game.players, MAX_PLAYERS);
	scene_init();

	game.channel = channel_open("GAME", 0, game_event_proc);

#if CLIENT
	game.tile_size = 24;
	config_get("game.tile_size", &game.tile_size);
#endif

#if SERVER
	//u32 num = random_int(2, 10);
	u32 num = 0;
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

	THINGS_FOREACH(&game.players)
	{
		Player* player = it;
		Unit* unit = scene_get_unit(player->controlled_unit);

		channel_reset(game.channel);
		channel_write_u8(game.channel, EVENT_Player_Join);
		channel_write_u32(game.channel, player->id);
		if (unit)
			channel_write_i32(game.channel, unit->id);
		else
			channel_write_i32(game.channel, -1);

		channel_send(game.channel, user, true);
	}

	Unit* player_unit = server_spawn_unit();
	player_unit->owner = user;

	channel_reset(game.channel);
	channel_write_u8(game.channel, EVENT_Player_Join);
	channel_write_u32(game.channel, user->id);
	channel_write_u32(game.channel, player_unit->id);
	channel_broadcast(game.channel, true);
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
	THINGS_FOREACH(&game.players)
	{
		player_update(it);
	}

#if CLIENT

	camera_update(&scene.camera);
	render_set_vp(camera_view_matrix(&scene.camera), camera_projection_matrix(&scene.camera));

#elif SERVER

	if (timer_update(&game.ai_spawn_timer))
	{
		//server_spawn_unit();
	}

#endif

	scene_update();
}