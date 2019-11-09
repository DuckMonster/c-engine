#include "Game.h"
#include "Engine/Render/Render.h"
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
			debug_log("Spawning unit %d, at (%f, %f)", unit_id, unit_position.x, unit_position.y);

			assert_msg(!scene.units[unit_id].active, "Received spawn unit %d, but it is already active!", unit_id);

			Unit* unit = unit_spawn(unit_id, unit_position);
			break;
		}

		case EVENT_Unit_Destroy:
		{
			u32 unit_id;
			channel_read(chnl, &unit_id);

			Unit* unit = scene.units + unit_id;
			assert_msg(unit->active, "Received destroy unit %d, but that unit isn't active", unit_id);

			unit_destroy(unit);
			break;
		}

		case EVENT_Player_Possess:
		{
#if CLIENT
			u32 unit_id;
			channel_read(chnl, &unit_id);
			assert_msg(scene.units[unit_id].active, "Received possess %d, but that unit is not active", unit_id);

			game.local_unit = scene.units + unit_id;
			scene.units[unit_id].is_local = true;
#endif
			break;
		}

		default:
			debug_log("Received unknown event type %d", event_type);
			break;
	}
}

#if SERVER
u32 server_spawn_unit()
{
	u32 unit_id = -1;
	for(u32 i=0; i<MAX_UNITS; ++i)
	{
		if (scene.units[i].active)
			continue;

		unit_id = i;
		break;
	}

	assert_msg(unit_id != -1, "Ran out of unit slots when spawning unit");

	Vec2 spawn_position;
	spawn_position.x = random_float(-5.f, 5.f);
	spawn_position.y = random_float(-5.f, 5.f);

	channel_reset(game.channel);
	channel_write_u8(game.channel, EVENT_Unit_Spawn);
	channel_write_u32(game.channel, unit_id);
	channel_write_vec2(game.channel, spawn_position);
	channel_broadcast(game.channel, true);

	return unit_id;
}

void server_destroy_unit(u32 unit_id)
{
	assert_msg(scene.units[unit_id].active, "Trying to destroy unit that isn't active");

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
	for(u32 i=0; i<MAX_UNITS; ++i)
	{
		Unit* unit = scene.units + i;
		if (!unit->active)
			continue;

		channel_reset(game.channel);
		channel_write_u8(game.channel, EVENT_Unit_Spawn);
		channel_write_u32(game.channel, unit->id);
		channel_write_vec2(game.channel, unit->position);
		channel_send(game.channel, user, true);
	}

	u32 player_unit = server_spawn_unit();

	scene.units[player_unit].owner = user;
	channel_reset(game.channel);
	channel_write_u8(game.channel, EVENT_Player_Possess);
	channel_write_u32(game.channel, player_unit);
	channel_send(game.channel, user, true);
}

void game_user_leave(Online_User* user)
{
	u32 owned_unit = -1;
	for(u32 i=0; i<MAX_UNITS; ++i)
	{
		if (scene.units[i].owner == user)
		{
			owned_unit = i;
			break;
		}
	}

	assert(owned_unit != -1);
	server_destroy_unit(owned_unit);
}
#endif

void game_update()
{
#if CLIENT
	if (game.local_unit)
		player_control(game.local_unit);
	camera_update(&scene.camera);
	render_set_vp(camera_view_matrix(&scene.camera), camera_projection_matrix(&scene.camera));
#endif

#if SERVER
	for(u32 i=0; i<MAX_UNITS; ++i)
	{
		Unit* unit = scene.units + i;
		if (!unit->active || unit->owner)
			continue;

		unit->walk_timer -= time_delta();
		if (unit->walk_timer < 0.f)
		{
			unit->ai_walk_target.x = random_float(-5.f, 5.f);
			unit->ai_walk_target.y = random_float(-5.f, 5.f);
			unit->walk_timer = random_float(2.f, 6.f);
		}
	}
#endif

	units_update();
	projectiles_update();
}