#pragma once
#include "Runtime/Game/HandleTypes.h"
#include "WeaponType.h"

struct Billboard;

const float weapon_interp_speed = 14.2f;
const float weapon_rotate_speed = 15.f;
const float weapon_hold_distance = 0.8f;

const float weapon_offset_drag = 10.5f;
const float weapon_offset_acceleration = 210.f;

const float weapon_angle_offset_drag = 10.5f;
const float weapon_angle_offset_acceleration = 100.f;

struct Weapon
{
	Unit_Handle owner;
	Vec3 position;

	Weapon_Type type;
	void* weapon_type_ptr = nullptr;

#if CLIENT
	Billboard* billboard = nullptr;

	Vec3 offset_velocity = Vec3_Zero;
	Vec3 offset = Vec3_Zero;
	float angle_offset = 0.f;
	float angle_offset_velocity = 0.f;
#endif
};

void weapon_init(Weapon* weapon, Unit* owner, const Weapon_Instance& instance);
void weapon_free(Weapon* weapon);
void weapon_update(Weapon* weapon);

#if CLIENT
void weapon_reset_offset(Weapon* weapon);
void weapon_add_velocity(Weapon* weapon, const Vec3& linear_velocity, float angular_velocity);
inline void weapon_add_linear_velocity(Weapon* weapon, const Vec3& velocity) { weapon_add_velocity(weapon, velocity, 0.f); }
inline void weapon_add_angular_velocity(Weapon* weapon, float velocity) { weapon_add_velocity(weapon, Vec3_Zero, velocity); }
#endif