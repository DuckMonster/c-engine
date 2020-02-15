#include "Game.h"
#include "Core/Input/Input.h"
#include "Core/Context/Context.h"
#include "Engine/Config/Config.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/MeshResource.h"
#include "Runtime/Render/Render.h"
#include "Runtime/Render/Drawable.h"
#include "Runtime/Render/ColorMap.h"
#include "Runtime/Game/Scene.h"
#include "Runtime/Online/Online.h"
#include "Runtime/Unit/Unit.h"
#include "Runtime/Player/Player.h"
#include "Runtime/Mobs/Mob.h"
#include "Runtime/Weapon/WeaponType.h"

Game game;

enum Game_Event
{
	EVENT_Unit_Spawn,
	EVENT_Unit_Destroy,
	EVENT_Player_Join,
	EVENT_Player_Leave,
	EVENT_Mob_Create,
	EVENT_Mob_Destroy,
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
			// Set local player if this is it
			if (client_is_self(player_id))
				game.local_player = player;
#endif

			break;
		}

		case EVENT_Player_Leave:
		{
			u32 player_id;
			channel_read(chnl, &player_id);

			Player* player = game.players[player_id];
			assert(player);

			player_free(player);
			thing_remove(&game.players, player);
			break;
		}

		case EVENT_Mob_Create:
		{
			u32 mob_id;
			i32 unit_id;
			channel_read(chnl, &mob_id);
			channel_read(chnl, &unit_id);

			Unit_Handle unit_hndl;
			// If unit_id < 0, then the mob doesn't control a unit
			if (unit_id >= 0)
				unit_hndl = scene_unit_handle(unit_id);

			Mob* mob = thing_add_at(&game.mobs, mob_id);
			mob_init(mob, mob_id, unit_hndl);

			break;
		}

		case EVENT_Mob_Destroy:
		{
			u32 mob_id;
			channel_read(chnl, &mob_id);

			Mob* mob = game.mobs[mob_id];
			assert(mob);
			mob_free(mob);

			thing_remove(&game.mobs, mob);
			break;
		}

		default:
			debug_log("Received unknown event type %d", event_type);
			break;
	}
}

void game_init()
{
#if CLIENT
	color_map_init();
#endif

	weapon_types_load();

	thing_array_init(&game.players, MAX_PLAYERS);
	thing_array_init(&game.mobs, MAX_MOBS);

	scene_init();

	game.channel = channel_open("GAME", 0, game_event_proc);

#if CLIENT
	game.tile_size = 24;
	config_get("game.tile_size", &game.tile_size);

	game.floor = scene_make_drawable(mesh_load("Mesh/Environment/floor.fbx"), material_load("Material/Environment/floor.mat"));
	editor_init();

	scene_make_item_drop(Vec3(0.2f, -5.f, 0.f), weapon_instance_make(WEAPON_Pistol, 0));
	scene_make_item_drop(Vec3(1.2f, -5.f, 0.f), weapon_instance_make(WEAPON_AssaultRifle, 0));
#endif

#if SERVER
	game.ai_spawn_timer.interval = 10.f;
	game.ai_spawn_timer.variance = 5.f;
#endif

	Prefab pref;
	prefab_load(&pref, "Prefab/main.prefab");
}

void game_update()
{
#if CLIENT
	// Toggle editor mode
	if (input_key_pressed(Key::Tab))
		game.is_editor = !game.is_editor;

	if (!game.is_editor)
	{
		camera_update(&game.camera);
		render_set_vp(camera_view_matrix(&game.camera), camera_projection_matrix(&game.camera));
	}
	else
	{
		editor_update();
	}

	// Time scaling
	if (input_key_pressed(Key::KeyPadPlus))
	{
		time_scale *= 1.5f;
		debug_log("Time Scale: %f", time_scale);
	}
	if (input_key_pressed(Key::KeyPadMinus))
	{
		time_scale /= 1.5f;
		debug_log("Time Scale: %f", time_scale);
	}

#elif SERVER

	if (thing_num(&game.players) > 0 &&
		thing_num(&game.mobs) < 0 &&
		timer_update(&game.ai_spawn_timer))
	{
		Vec2 position = random_point_on_circle();

		Unit* new_unit = game_spawn_unit_at(position * 20.f);
		Mob* mob = game_create_mob_for_unit(new_unit);

		Player* random_player = nullptr;
		while(random_player == nullptr)
		{
			u32 random_index = random_int(game.players.index_min, game.players.index_max);
			random_player = game.players[random_index];
		}

		Unit* player_unit = scene_get_unit(random_player->controlled_unit);
		if (player_unit)
			mob_set_agroo(mob, player_unit);
	}

#endif

	THINGS_FOREACH(&game.players)
		player_update(it);
	THINGS_FOREACH(&game.mobs)
		mob_update(it);

	scene_update();
}

#if CLIENT
void game_render(const Render_State& state)
{
	scene_render(state);

	if (game.is_editor)
		editor_render(state);
}
#endif

Player* game_get_player(const Player_Handle& player_hndl)
{
	return thing_resolve(&game.players, player_hndl);
}

Player_Handle game_player_handle(Player* player)
{
	return thing_get_handle(&game.players, player);
}

Mob* game_get_mob(const Mob_Handle& mob_hndl)
{
	return thing_resolve(&game.mobs, mob_hndl);
}

Mob_Handle game_mob_handle(Mob* mob)
{
	return thing_get_handle(&game.mobs, mob);
}

#if CLIENT
Mat4 game_ndc_to_pixel()
{
	float half_width = context.width / 2.f;
	float half_height = context.height / 2.f;
	return Mat4(
		half_width, 0.f, 0.f, 0.f,
		0.f, -half_height, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		half_width, half_height, 0.f, 1.f
	);
}

Ray game_mouse_ray()
{
	return game_screen_to_ray(Vec2(input_mouse_x(), input_mouse_y()));
}

Ray game_screen_to_ray(Vec2 screen)
{
	Mat4 pixel_to_ndc = inverse(game_ndc_to_pixel());
	Mat4 vp = camera_view_projection_matrix(&game.camera);
	vp = inverse(vp);

	Vec4 world_near = vp * pixel_to_ndc * Vec4(screen, -1.f, 1.f);
	world_near /= world_near.w;
	Vec4 world_far = vp * pixel_to_ndc * Vec4(screen, 1.f, 1.f);
	world_far /= world_far.w;

	Ray result;
	result.origin = (Vec3)world_near;
	result.direction = normalize(Vec3(world_far - world_near));

	return result;
}

Vec2 game_project_to_screen(const Vec3& position)
{
	Mat4 ndc_to_pixel = game_ndc_to_pixel();
	Mat4 view_projection = camera_view_projection_matrix(&game.camera);

	return Vec2(ndc_to_pixel * view_projection * Vec4(position, 1.f));
}
#endif

#if SERVER
Unit* game_spawn_random_unit()
{
	Vec2 spawn_position;
	spawn_position.x = random_float(-5.f, 5.f);
	spawn_position.y = random_float(-5.f, 5.f);

	return game_spawn_unit_at(spawn_position);
}

Unit* game_spawn_unit_at(const Vec2& position)
{
	u32 unit_id = scene_get_free_unit_id();

	channel_reset(game.channel);
	channel_write_u8(game.channel, EVENT_Unit_Spawn);
	channel_write_u32(game.channel, unit_id);
	channel_write_vec2(game.channel, position);
	channel_broadcast(game.channel, true);

	return scene.units[unit_id];
}

void game_destroy_unit(Unit* unit)
{
	channel_reset(game.channel);
	channel_write_u8(game.channel, EVENT_Unit_Destroy);
	channel_write_u32(game.channel, unit->id);
	channel_broadcast(game.channel, true);
}

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

	THINGS_FOREACH(&game.mobs)
	{
		Mob* mob = it;
		Unit* unit = scene_get_unit(mob->controlled_unit);

		channel_reset(game.channel);
		channel_write_u8(game.channel, EVENT_Mob_Create);
		channel_write_u32(game.channel, mob->id);
		if (unit)
			channel_write_i32(game.channel, unit->id);
		else
			channel_write_i32(game.channel, -1);

		channel_send(game.channel, user, true);
	}

	Unit* player_unit = game_spawn_random_unit();

	channel_reset(game.channel);
	channel_write_u8(game.channel, EVENT_Player_Join);
	channel_write_u32(game.channel, user->id);
	channel_write_u32(game.channel, player_unit->id);
	channel_broadcast(game.channel, true);
}

void game_user_leave(Online_User* user)
{
	Player* player = game.players[user->id];
	assert(player);

	// Destroy the unit controlled by the player (if there is one)
	Unit* owned_unit = scene_get_unit(player->controlled_unit);
	if (owned_unit)
		game_destroy_unit(owned_unit);

	// Destroy the player itself
	channel_reset(game.channel);
	channel_write_u8(game.channel, EVENT_Player_Leave);
	channel_write_u32(game.channel, user->id);
	channel_broadcast(game.channel, true);
}

Mob* game_create_mob_for_unit(Unit* unit)
{
	u32 mob_id = thing_find_first_free(&game.mobs);

	channel_reset(game.channel);
	channel_write_u8(game.channel, EVENT_Mob_Create);
	channel_write_u32(game.channel, mob_id);
	channel_write_u32(game.channel, unit->id);
	channel_broadcast(game.channel, true);

	// Hopefully, after the message has been broadcast, the mob shouldve been created
	Mob* mob = game.mobs[mob_id];
	assert(mob);

	return mob;
}

void game_destroy_mob(Mob* mob)
{
	channel_reset(game.channel);
	channel_write_u8(game.channel, EVENT_Mob_Destroy);
	channel_write_u32(game.channel, mob->id);
	channel_broadcast(game.channel, true);
}
#endif