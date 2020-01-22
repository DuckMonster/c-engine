#include "Player.h"
#include "Core/Input/Input.h"
#include "Engine/Collision/HitTest.h"
#include "Runtime/Unit/Unit.h"
#include "Runtime/Game/Scene.h"
#include "Runtime/Game/Game.h"
#include "Runtime/Online/Online.h"
#include "Runtime/Fx/Fx.h"

enum Player_Events
{
	EVENT_Set_Input,
	EVENT_Set_Aim_Position,
};

void player_event_proc(Channel* chnl, Online_User* src)
{
	Player* player = (Player*)chnl->user_ptr;

	u8 event_type;
	channel_read(chnl, &event_type);

	switch(event_type)
	{
		case EVENT_Set_Input:
		{
#if CLIENT
			if (player->is_local)
				return;
#endif

			Vec2 move_input;
			Vec2 position;
			channel_read(chnl, &move_input);
			channel_read(chnl, &position);

			player->move_input = move_input;

			Unit* unit = scene_get_unit(player->controlled_unit);
			if (unit)
			{
#if SERVER
				// On the server we want to snap the players, for more accurate mob behaviour
				unit->position = position;
#elif CLIENT
				// On clients though, it looks hella ugly, so use the error method instead
				player->net_position_error = position - unit->position;
#endif
			}

#if SERVER
			channel_rebroadcast_last(chnl, false);
#endif
			break;
		}
	}
}

void player_init(Player* player, u32 id, const Unit_Handle& unit_to_control)
{
	player->id = id;
	player->controlled_unit = unit_to_control;

	player->channel = channel_open("PLYR", id, player_event_proc);
	player->channel->user_ptr = player;

	Unit* unit = scene_get_unit(unit_to_control);

#if CLIENT
	if (client_is_self(id))
	{
		player->is_local = true;
		if (unit)
			unit->player_owner = game_player_handle(player);
	}

	player->force_sync_timer.interval = 1.f / player_force_sync_frequency;
#endif
}

void player_free(Player* player)
{
	channel_close(player->channel);
}

#if CLIENT
void player_update_local_input(Player* player)
{
	Unit* unit = scene_get_unit(player->controlled_unit);

	/* Calculate input */
	Vec2 forward;
	forward = (Vec2)camera_forward(&game.camera);
	forward = normalize(forward);

	Vec2 right;
	right = Vec2(forward.y, -forward.x);

	Vec2 input;
	if (input_key_down(Key::D))
		input.x += 1.f;
	if (input_key_down(Key::A))
		input.x -= 1.f;
	if (input_key_down(Key::W))
		input.y += 1.f;
	if (input_key_down(Key::S))
		input.y -= 1.f;

	Vec2 direction = right * input.x + forward * input.y;
	if (!is_nearly_zero(direction))
		direction = normalize(direction);

	// Input has changed this frame, so sync what 
	if (!is_nearly_equal(direction, player->move_input) || timer_update(&player->force_sync_timer))
	{
		channel_reset(player->channel);
		channel_write_u8(player->channel, EVENT_Set_Input);
		channel_write_vec2(player->channel, direction);
		channel_write_vec2(player->channel, unit->position);
		channel_broadcast(player->channel, false);
	}

	player->move_input = direction;

	/* Calculate mouse position */
	Ray mouse_ray = game_mouse_ray();
	Plane ground_plane;
	ground_plane.normal = Vec3_Z;
	ground_plane.point = Vec3(0.f, 0.f, 0.5f);

	Hit_Result ground_hit = test_ray_plane(mouse_ray, ground_plane);
	player->aim_position = Vec2(ground_hit.position);

	/* Shooting */
	if (input_mouse_button_pressed(Mouse_Btn::Left))
	{
		unit_shoot(unit, player->aim_position);
	}
}
#endif

void player_update(Player* player)
{
	Unit* unit = scene_get_unit(player->controlled_unit);
	if (unit == nullptr)
		return;

	if (unit_has_control(unit))
	{
#if CLIENT
		if (!game.is_editor)
			player_update_local_input(player);

		if (input_key_pressed(Key::F))
		{
			Fx_Particle_Spawn_Params params;
			params.num_particles = 10;
			params.position = Vec3(unit->position, 0.5f);
			params.position_radius = 0.f;
			params.velocity = (-Vec3_X + Vec3_Z) * 4.f;
			params.velocity_cone_angle = 5.f;
			params.velocity_scale_variance = 0.0f;
			fx_make_particle(params);
		}
#endif
	}
	else
	{
		Vec2 net_correction_delta = player->net_position_error * player_net_correction_coefficient * time_delta();
		player->net_position_error -= net_correction_delta;
		unit->position += net_correction_delta;
	}

	unit_move_direction(unit, player->move_input);
	unit->aim_direction = normalize_safe(player->aim_position - unit->position);
}