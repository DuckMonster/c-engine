#pragma once
#include "Core/Time/IntervalTimer.h"
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
const Vec3 unit_center_offset = Vec3(0.f, 0.f, 0.5f);

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
void unit_move_delta(Unit* unit, const Vec3& delta);
void unit_hit(Unit* unit, const Unit_Handle& source, float damage, const Vec3& impulse);
bool unit_has_control(Unit* unit);

void unit_equip_weapon(Unit* unit, const Weapon_Instance& data);
