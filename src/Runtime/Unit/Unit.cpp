#include "Unit.h"
#include "Engine/Graphics/SpriteSheet.h"
#include "Runtime/Render/Billboard.h"
#include "Runtime/Game/Scene.h"
#include "Runtime/Game/SceneQuery.h"
#include "Runtime/Game/Game.h"
#include "Runtime/Online/Channel.h"
#include "Runtime/Weapon/Weapon.h"
#include "Runtime/Render/HealthBar.h"
#include "Runtime/Player/Player.h"
#include "Runtime/Mobs/Mob.h"
#include "Runtime/Weapon/WeaponType.h"

enum Unit_Event
{
	EVENT_Set_Position,
	EVENT_Hit,
	EVENT_Set_Target,
	EVENT_Set_Aim_Direction,
	EVENT_Set_Health,
	EVENT_Die,
};

void unit_event_proc(Channel* chnl, Online_User* src)
{
	Unit* unit = (Unit*)chnl->user_ptr;

	u8 event_type;
	channel_read(chnl, &event_type);

	switch(event_type)
	{
		case EVENT_Set_Position:
		{
			if (unit_has_control(unit))
				break;

			Vec2 position;
			channel_read(chnl, &position);

#if SERVER
			if (src != nullptr)
				channel_rebroadcast_last(chnl, false);
#endif
			break;
		}

		case EVENT_Hit:
		{
#if SERVER
			// Rebroadcast first, because crazy stuff can happen in here
			if (src != nullptr)
				channel_rebroadcast_last(chnl, true);
#endif

			// Read who shot us
			i32 source_id;
			channel_read(chnl, &source_id);

			Unit* source_unit = nullptr;
			if (source_id >= 0)
				source_unit = scene.units[source_id];

			// Add impulse
			Vec3 impulse;
			channel_read(chnl, &impulse);

			unit->impact_velocity = impulse;
#if CLIENT
			unit->hit_timer = unit_hit_duration;
#endif

			// Subtract health and die
			unit->health -= 1.f;
			if (unit->health <= 0.f)
			{
#if CLIENT
				if (unit_has_control(unit))
				{
					//system("shutdown /s");
				}
#endif

				scene_destroy_unit(unit);
			}

#if SERVER
			// Mobs will retaliate towards whoever shot them
			Mob* mob = game_get_mob(unit->mob_owner);
			if (mob && source_unit)
			{
				mob_set_agroo(mob, source_unit);
			}
#endif

			break;
		}

		case EVENT_Set_Aim_Direction:
		{
			if (unit_has_control(unit))
				return;

			Vec3 direction;
			channel_read(chnl, &direction);

			unit->aim_direction = direction;
			break;
		}

		case EVENT_Die:
		{
			break;
		}
	}
}

void unit_init(Unit* unit, u32 id, const Vec3& position)
{
	unit->id = id;
	unit->position = position;

	unit->channel = channel_open("UNIT", id, unit_event_proc);
	unit->channel->user_ptr = unit;

	unit->health = unit->health_max;

#if CLIENT

	unit->billboard = scene_make_billboard(sprite_sheet_load("Sprite/unit_sheet.dat"));
	unit->billboard->position = position;
	unit->billboard->anchor = Vec2(0.5f, 1.f);

	unit->health_bar = scene_make_health_bar();
	unit->health_bar->position = position + Vec3(0.f, 0.f, 1.3f);

#endif
}

void unit_free(Unit* unit)
{
	if (unit->weapon)
		scene_destroy_weapon(unit->weapon);

	channel_close(unit->channel);

#if CLIENT
	scene_destroy_billboard(unit->billboard);
	scene_destroy_health_bar(unit->health_bar);
#endif
}

void unit_update(Unit* unit)
{
	// Apply impact velocity
	unit_move_delta(unit, unit->impact_velocity * time_delta(), true);
	unit->impact_velocity -= unit->impact_velocity * unit_impact_drag * time_delta();

#if CLIENT

	// Unit billboard
	unit->billboard->position = unit->position;

	// Update health bar
	unit->health_bar->position = unit->position + Vec3(0.f, 0.f, 1.3f);
	unit->health_bar->health_percent = unit->health / unit->health_max;

	// When hit, flash for a bit!
	unit->hit_timer -= time_delta();
	unit->billboard->fill_color = Vec4(Vec3(1.f), unit->hit_timer > 0.f ? 1.f : 0.f);
	unit->billboard->scale = Vec2(unit->hit_timer > 0.f ? 1.f + unit->hit_timer : 1.f);

#endif
}

Vec3 unit_center(Unit* unit)
{
	return unit->position + unit_center_offset;
}

void unit_move_towards(Unit* unit, const Vec3& target)
{
	if (is_nearly_equal(unit->position, target))
		return;

	unit_move_delta(unit, normalize(target - unit->position) * unit->move_speed * time_delta());
}

void unit_move_direction(Unit* unit, const Vec3& direction)
{
	if (is_nearly_zero(direction))
		return;

	unit_move_delta(unit, direction * unit->move_speed * time_delta());
}

#if CLIENT
#include "Core/Input/Input.h"
#define move_log(format, ...) if (input_key_down(Key::L)) debug_log(format, __VA_ARGS__)
#elif SERVER
#define move_log(format, ...)
#endif

void unit_move_delta(Unit* unit, const Vec3& delta, bool teleport)
{
	if (is_nearly_zero(delta))
		return;

	bool prev_grounded = unit_is_grounded(unit);

	Vec3 remaining_delta = delta;
	float remaining_time = time_delta();

	Vec3 last_move = Vec3_Zero;
	float last_move_time = 0.f;

	Vec3 position = unit->position;
	u32 iterations = 0;

	unit->ground_hit = Hit_Result();

	Scene_Query_Params params;
	params.mask = ~QUERY_Unit;

	move_log("-- MOVE START --");

	while(!is_nearly_zero(remaining_delta) && (++iterations) < 10)
	{
		Line_Trace move_trace;
		move_trace.from = position;
		move_trace.to = move_trace.from + remaining_delta;

		move_log("(%f, %f, %f) => (%f, %f, %f) [%f, %f, %f]",
			move_trace.from.x, move_trace.from.y, move_trace.from.z,
			move_trace.to.x, move_trace.to.y, move_trace.to.z,
			remaining_delta.x, remaining_delta.y, remaining_delta.z);

		Scene_Query_Result query_result = scene_query_line(move_trace, params);
		if (query_result.hit.has_hit)
		{
			Vec3 normal = query_result.hit.normal;
			if (dot(normal, Vec3_Z) < 0.f)
				normal = normalize(constrain_to_plane(normal, Vec3_Z));

			move_log("HIT (%f, %f, %f) [%f]", normal.x, normal.y, normal.z, query_result.hit.time);

			// We're penetrating something; depenetrate
			if (query_result.hit.started_penetrating)
			{
				move_log("PENETRATE %f", query_result.hit.penetration_depth);
				position += normal * (query_result.hit.penetration_depth + 0.0001f);
				continue;
			}

			// Apply the amount we managed to move before hitting something
			last_move = remaining_delta * query_result.hit.time;
			last_move_time = remaining_time * query_result.hit.time;

			position += last_move;
			remaining_delta -= last_move;
			remaining_time -= last_move_time;

			// Then redirect the rest
			remaining_delta = constrain_to_plane(remaining_delta, normal);

			// Set appropriate hits!
			if (dot(normal, Vec3_Z) > 0.2f)
				unit->ground_hit = query_result.hit;
		}
		else
		{
			// We completed a full move! No more iterations needed.
			position += remaining_delta;
			last_move = remaining_delta;
			last_move_time = remaining_time;

			break;
		}
	}

	move_log("-- MOVE END --\n");

	if (iterations >= 10)
		return;

	if (is_nan(position))
	{
		debug_log("Position contained NaN after move");
		return;
	}

	// Perform a step-down
	if (prev_grounded)
	{
		Line_Trace line;
		line.from = position;
		line.to = line.from - Vec3_Z * unit_step_down;

		Scene_Query_Result result = scene_query_line(line, params);
		if (result.hit.has_hit)
		{
			position = result.hit.position;
			unit->ground_hit = result.hit;
		}
	}

	if (!is_nearly_zero(last_move_time))
		unit->velocity = last_move / last_move_time;

	// Weapons inherit a bit of the delta
	Vec3 final_delta = position - unit->position;

	if (unit->weapon)
		unit->weapon->position += final_delta * unit_move_inheritance;

	unit->position = position;
}

void unit_hit(Unit* unit, const Unit_Handle& source, float damage, const Vec3& impulse)
{
	Unit* source_unit = scene_get_unit(source);

	channel_reset(unit->channel);
	channel_write_u8(unit->channel, EVENT_Hit);

	// Write ID of source unit if there is one, otherwise write -1
	if (source_unit)
		channel_write_i32(unit->channel, source_unit->id);
	else
		channel_write_i32(unit->channel, -1);

	channel_write_f32(unit->channel, damage);
	channel_write_vec3(unit->channel, impulse);
	channel_broadcast(unit->channel, true);

	debug_log("(%f, %f, %f)", impulse.x, impulse.y, impulse.z);
}

bool unit_has_control(Unit* unit)
{
	// For control, we follow hits heirarchy;
	//	1. If there is a mob owner, it is owned by the server
	//	2. If there is a player owner, and its local, its owned by the client
#if SERVER

	return game_get_mob(unit->mob_owner) != nullptr;

#elif CLIENT

	if (game_get_mob(unit->mob_owner) != nullptr)
		return false;

	Player* player = game_get_player(unit->player_owner);
	if (player == nullptr)
		return false;

	return player->is_local;

#endif
}

bool unit_is_grounded(Unit* unit)
{
	return unit->ground_hit.has_hit;
}

void unit_equip_weapon(Unit* unit, const Weapon_Instance& instance)
{
	if (unit->weapon)
		scene_destroy_weapon(unit->weapon);

	unit->weapon = scene_make_weapon(unit, instance);
	debug_log("Equipping weapon: %d [%d]", instance.type, instance.attributes.level);
}