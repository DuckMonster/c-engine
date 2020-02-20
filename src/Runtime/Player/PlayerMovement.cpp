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
	if (input_key_pressed(Key::Spacebar) && !is_nearly_zero(direction))
	{
		movement.is_dashing = true;

		// Set velocity to be the impulse
		unit->velocity = Vec3(direction * player_dash_hori_impulse, player_dash_vert_impulse);
		unit->ground_hit = Hit_Result();
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
	Vec3 velocity = unit->velocity;

	// Gravity
	if (!unit_is_grounded(unit))
		velocity -= Vec3_Z * player_gravity * time_delta();

	if (movement.is_dashing)
	{
		velocity -= Vec3(velocity.x, velocity.y, 0.f) * player_dash_drag * time_delta();
	}

	if (!movement.is_dashing)
	{
		velocity += Vec3(movement.move_input, 0.f) * 80.f * time_delta();
		if (!unit_is_grounded(unit))
			velocity -= constrain_to_plane(velocity, Vec3_Z) * 12.f * time_delta();
		else
			velocity -= velocity * 12.f * time_delta();
	}

	unit_move_delta(unit, velocity * time_delta());

	if (unit->ground_hit.has_hit)
		movement.is_dashing = false;
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