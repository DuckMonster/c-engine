#include "PlayerMovement.h"
#include "Player.h"
#include "Runtime/Game/Scene.h"
#include "Runtime/Game/Game.h"
#include "Runtime/Unit/Unit.h"
#include "Core/Input/Input.h"

void player_movement_init(Player* player)
{
	Player_Movement& movement = player->movement;

#if CLIENT
	movement.force_sync_timer.interval = 1.f / player_force_sync_frequency;
#endif
}

#if CLIENT
void player_movement_update_local(Player* player)
{
	if (game.is_editor)
		return;

	Unit* unit = scene_get_unit(player->controlled_unit);
	Player_Movement& movement = player->movement;

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
	if (!is_nearly_equal(direction, movement.move_input) || timer_update(&movement.force_sync_timer))
	{
		channel_reset(player->channel);
		channel_write_u8(player->channel, PLAYEREV_Set_Input);
		channel_write_vec2(player->channel, direction);
		channel_write_vec3(player->channel, unit->position);
		channel_broadcast(player->channel, false);
	}

	// Dashing!
	if (input_key_pressed(Key::LeftShift) && !is_nearly_zero(direction))
	{
		movement.is_dashing = true;
		movement.dash_velocity = Vec3(direction * player_dash_hori_impulse, player_dash_vert_impulse);
	}

	movement.move_input = direction;
}
#endif

void player_movement_update(Player* player)
{
	Unit* unit = scene_get_unit(player->controlled_unit);
	Player_Movement& movement = player->movement;

#if CLIENT
	if (player->is_local)
	{
		player_movement_update_local(player);
	}
	else
	{
		Vec3 net_correction_delta = movement.net_position_error * player_net_correction_coefficient * time_delta();
		movement.net_position_error -= net_correction_delta;
		unit->position += net_correction_delta;
	}
#endif

	// Dashing
	if (movement.is_dashing)
	{
		movement.dash_velocity -= movement.dash_velocity * player_dash_drag * time_delta();
		movement.dash_velocity -= Vec3(0.f, 0.f, player_dash_gravity) * time_delta();

		debug_log("(%f, %f, %f)", movement.dash_velocity.x, movement.dash_velocity.y, movement.dash_velocity.z);
		unit_move_delta(unit, movement.dash_velocity * time_delta());

		if (movement.dash_velocity.z < 0.f && unit->position.z < 0.1f)
		{
			unit->position.z = 0.f;
			movement.is_dashing = false;
		}
	}
	else
	{
		unit_move_direction(unit, Vec3(movement.move_input, 0.f));
	}
}

void player_movement_recv_remote_input(Player* player, const Vec2& remote_input, const Vec3& remote_position)
{
	Player_Movement& movement = player->movement;
	movement.move_input = remote_input;

	Unit* unit = scene_get_unit(player->controlled_unit);
	if (unit)
	{
#if SERVER
		// On the server we want to snap the players, for more accurate mob behaviour
		unit->position = remote_position;
#elif CLIENT
		// On clients though, it looks hella ugly, so use the error method instead
		movement.net_position_error = remote_position - unit->position;
#endif
	}
}