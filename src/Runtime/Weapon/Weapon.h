#pragma once
#include "Runtime/Game/HandleTypes.h"
struct Billboard;

const float weapon_interp_speed = 14.2f;
const float weapon_rotate_speed = 15.f;
const float weapon_hold_distance = 0.8f;

const float weapon_offset_drag = 10.5f;
const float weapon_offset_acceleration = 210.f;
const float weapon_shoot_impulse = 12.f;

const float weapon_angle_offset_drag = 10.5f;
const float weapon_angle_offset_acceleration = 100.f;
const float weapon_shoot_angle_impulse = 7.f;

struct Weapon
{
	Unit_Handle owner;
	Vec2 position;

#if CLIENT
	Billboard* billboard = nullptr;

	Vec2 offset_velocity = Vec2_Zero;
	Vec2 offset = Vec2_Zero;
	float angle_offset = 0.f;
	float angle_offset_velocity = 0.f;
#endif
};

void weapon_init(Weapon* weapon, Unit* owner);
void weapon_free(Weapon* weapon);
void weapon_update(Weapon* weapon);
void weapon_shoot(Weapon* weapon, const Vec2& target);