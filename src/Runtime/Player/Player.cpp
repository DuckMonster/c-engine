#include "Player.h"
#include "Core/Input/Input.h"
#include "Runtime/Unit/Unit.h"
#include "Runtime/Game/Scene.h"
#include "Runtime/Online/Online.h"

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
				player->net_position_error = position - unit->position;

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

	Unit* unit = scene_get_unit(unit_to_control);
	assert(unit);

	player->channel = channel_open("PLYR", id, player_event_proc);
	player->channel->user_ptr = player;

#if CLIENT
	if (client_is_self(id))
	{
		player->is_local = true;
		unit->is_local = true;
	}

	player->force_sync_timer.interval = 1.f / player_force_sync_frequency;
#endif
}

#if CLIENT
void player_update_local_input(Player* player)
{
	Unit* unit = scene_get_unit(player->controlled_unit);

	/* Calculate input */
	Vec2 forward;
	forward = (Vec2)camera_forward(&scene.camera);
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
	Ray mouse_ray = scene_mouse_ray();
	player->aim_position = Vec2(ray_plane_intersect(mouse_ray, Vec3(0.f, 0.f, 0.5f), Vec3_Z));

	/* Shooting */
	if (input_mouse_button_pressed(Mouse_Btn::Left))
	{
		Ray mouse_ray = scene_mouse_ray();
		Vec3 mouse_pos = ray_plane_intersect(mouse_ray, Vec3(0.f, 0.f, 0.5f), Vec3_Z);

		unit_shoot(unit, (Vec2)mouse_pos);
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
		player_update_local_input(player);
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