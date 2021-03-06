#pragma once
#include "Core/Time/IntervalTimer.h"
#include "Engine/Collision/HitTest.h"
#include "Runtime/Game/HandleTypes.h"

struct Billboard;
struct Channel;
struct Online_User;
struct Health_Bar;
struct Weapon;
struct Weapon_Instance;

const float unit_impact_drag = 8.5f;
const float unit_hit_duration = 0.15f;
const float unit_sync_frequency = 10.f;
const float unit_move_inheritance = 0.85f;

const float unit_death_hori_impulse = 12.f;
const float unit_death_vert_impulse = 4.f;
const float unit_death_gravity = 14.f;
const float unit_death_friction = 0.4f;
const float unit_death_bounce = 0.4f;

const Vec3 unit_center_offset = Vec3(0.f, 0.f, 0.5f);

const float unit_step_down = 0.2f;

struct Unit
{
	u32 id;
	Vec3 position;
	Vec3 aim_direction = Vec3_X;
	Vec3 impact_velocity;
	float move_speed = 6.f;

	Weapon* weapon = nullptr;
	Channel* channel = nullptr;

	float health;
	float health_max = 5.f;

	Player_Handle player_owner;
	Mob_Handle mob_owner;

	// Calculated after each move
	Vec3 velocity;
	Hit_Result ground_hit;

	float death_timer = 0.f;

#if CLIENT
	Billboard* billboard = nullptr;
	Health_Bar* health_bar = nullptr;

	float hit_timer = 0.f;
#endif
};

void unit_init(Unit* unit, u32 id, const Vec3& position);
void unit_free(Unit* unit);
void unit_update(Unit* unit);

Vec3 unit_center(Unit* unit);

void unit_move_towards(Unit* unit, const Vec3& target);
void unit_move_direction(Unit* unit, const Vec3& direction);
void unit_move_delta(Unit* unit, const Vec3& delta, bool teleport = false);

void unit_hit(Unit* unit, const Unit_Handle& source, float damage, const Vec3& impulse);
bool unit_has_control(Unit* unit);
bool unit_is_alive(Unit* unit);

bool unit_is_grounded(Unit* unit);

void unit_equip_weapon(Unit* unit, const Weapon_Instance& data);
