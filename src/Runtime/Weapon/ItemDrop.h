#pragma once
#include "Runtime/Render/Render.h"
#include "Runtime/Weapon/WeaponType.h"

struct Item_Drop
{
	Vec3 position;
	Weapon_Instance weapon;

#if CLIENT
	bool hovered = false;
#endif
};

void item_drop_init(Item_Drop* drop, const Vec3& position, const Weapon_Instance& weapon);
void item_drop_free(Item_Drop* drop);
Item_Drop* item_drop_get_closest(const Vec3& position, float max_radius);

#if CLIENT
void item_drop_render(Item_Drop* drop, const Render_State& state);
#endif