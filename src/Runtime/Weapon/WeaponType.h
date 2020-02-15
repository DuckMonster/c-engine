#pragma once

enum Weapon_Type
{
	WEAPON_Pistol,
	WEAPON_AssaultRifle,
	WEAPON_MAX,
};

struct Weapon_Type_Data
{
	u32 tile_x;
	u32 tile_y;
};

struct Weapon_Attributes
{
	u8 level;
};

struct Weapon_Instance
{
	Weapon_Type type;
	Weapon_Attributes attributes;
};

Weapon_Instance weapon_instance_make(Weapon_Type type, u8 level);

void weapon_types_load();
const Weapon_Type_Data* weapon_get_type_data(Weapon_Type type);
