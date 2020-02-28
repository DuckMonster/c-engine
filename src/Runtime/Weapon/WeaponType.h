#pragma once

enum Weapon_Type
{
	WEAPON_Pistol,
	WEAPON_AssaultRifle,
	WEAPON_Shotgun,
	WEAPON_MAX,
};

enum Weapon_Fire_Mode
{
	FIREMODE_Press,
	FIREMODE_Auto,
};

struct Weapon_Type_Data
{
	u32 tile_x;
	u32 tile_y;

	float damage;

	u32 projectile_num = 1;
	float projectile_size = 0.3f;
	float projectile_spread = 0.f;
	float projectile_speed;
	float projectile_speed_variance = 0.f;
	float projectile_drag = 0.f;
	float projectile_gravity = 0.f;

	float muzzle_offset = 0.5f;

	u32 fire_mode;
	float fire_rate;

	float recoil_gain;
	float recoil_decay;

	float muzzle_fx_scale = 1.f;
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

void weapon_types_load();
const Weapon_Type_Data* weapon_get_type_data(Weapon_Type type);

Weapon_Instance weapon_instance_make(Weapon_Type type, u8 level);
