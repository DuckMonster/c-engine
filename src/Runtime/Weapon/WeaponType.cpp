#include "WeaponType.h"

static Weapon_Type_Data type_data[WEAPON_MAX];

Weapon_Instance weapon_instance_make(Weapon_Type type, u8 level)
{
	Weapon_Instance instance;
	instance.type = type;
	instance.attributes.level = level;

	return instance;
}

static Weapon_Type_Data data_make(u32 tile_x, u32 tile_y)
{
	Weapon_Type_Data data;
	data.tile_x = tile_x;
	data.tile_y = tile_y;

	return data;
}

void weapon_types_load()
{
	type_data[WEAPON_Pistol] = data_make(0, 0);
	type_data[WEAPON_AssaultRifle] = data_make(1, 0);
}

const Weapon_Type_Data* weapon_get_type_data(Weapon_Type type)
{
	return &type_data[type];
}