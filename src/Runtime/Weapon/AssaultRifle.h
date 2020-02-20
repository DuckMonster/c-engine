#pragma once
#include "WeaponType.h"
struct Channel;
struct Weapon;

const float assault_rifle_fire_rate = 8.f;
const float assault_rifle_spread_accum = 0.7f;
const float assault_rifle_spread_decrease = 1.2f;

struct Assault_Rifle
{
	Weapon* weapon;
	Channel* channel;

	Weapon_Attributes attributes;

	bool trigger_held = false;
	Vec3 fire_target;
	float last_fire_time = 0.f;
	float spread_degrees = 0.f;
};

void assault_rifle_init(Assault_Rifle* rifle, Weapon* weapon, const Weapon_Attributes& attributes);
void assault_rifle_free(Assault_Rifle* rifle);
void assault_rifle_update(Assault_Rifle* rifle);

// Call every frame you want to fire
void assault_rifle_fire(Assault_Rifle* rifle, const Vec3& target);