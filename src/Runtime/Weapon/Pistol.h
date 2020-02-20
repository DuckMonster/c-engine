#pragma once
#include "WeaponType.h"
struct Channel;
struct Weapon;

struct Pistol
{
	Weapon* weapon = nullptr;
	Channel* channel;

	Weapon_Attributes attributes;
};

void pistol_init(Pistol* pistol, Weapon* weapon, const Weapon_Attributes& attributes);
void pistol_free(Pistol* pistol);
void pistol_fire(Pistol* pistol, const Vec3& target);