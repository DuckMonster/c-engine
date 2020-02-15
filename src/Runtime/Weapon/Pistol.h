#pragma once
#include "WeaponType.h"
struct Channel;
struct Weapon;

struct Pistol
{
	Weapon* weapon = nullptr;

	Weapon_Attributes attributes;
	Channel* channel;
};

void pistol_init(Pistol* pistol, Weapon* weapon, const Weapon_Attributes& attributes);
void pistol_free(Pistol* pistol);
void pistol_fire_net(Pistol* pistol, const Vec2& target);
void pistol_fire(Pistol* pistol, const Vec2& origin, const Vec2& target);