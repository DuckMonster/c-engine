#include "WeaponType.h"
#include "Core/Import/Dat.h"
#include "Engine/Resource/Resource.h"

static const Weapon_Type_Data* type_data[WEAPON_MAX];

void weapon_res_create(Resource* resource)
{
	Weapon_Type_Data* data = (Weapon_Type_Data*)resource->ptr;
	if (!data)
	{
		data = new Weapon_Type_Data();
		resource->ptr = data;
	}

	Dat_Document doc;
	if (!dat_load_file(&doc, resource->path))
		error("Failed to load weapon-type file '%s'", resource->path);

	dat_read(doc.root, "tile[0]", &data->tile_x);
	dat_read(doc.root, "tile[1]", &data->tile_y);

	dat_read(doc.root, "damage", &data->damage);

	dat_read(doc.root, "projectile_num", &data->projectile_num);
	dat_read(doc.root, "projectile_size", &data->projectile_size);
	dat_read(doc.root, "projectile_spread", &data->projectile_spread);
	dat_read(doc.root, "projectile_speed", &data->projectile_speed);
	dat_read(doc.root, "projectile_speed_variance", &data->projectile_speed_variance);
	dat_read(doc.root, "projectile_drag", &data->projectile_drag);
	dat_read(doc.root, "projectile_gravity", &data->projectile_gravity);

	dat_read(doc.root, "muzzle_offset", &data->muzzle_offset);

	dat_read(doc.root, "fire_mode", &data->fire_mode);
	dat_read(doc.root, "fire_rate", &data->fire_rate);

	dat_read(doc.root, "recoil_gain", &data->recoil_gain);
	dat_read(doc.root, "recoil_decay", &data->recoil_decay);

	dat_read(doc.root, "muzzle_fx_scale", &data->muzzle_fx_scale);
}

const Weapon_Type_Data* weapon_type_data_load(const char* path)
{
	Resource* resource = resource_load(path, weapon_res_create, nullptr);
	return (Weapon_Type_Data*)resource->ptr;
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
	type_data[WEAPON_Pistol] = weapon_type_data_load("Weapon/pistol.dat");
	type_data[WEAPON_AssaultRifle] = weapon_type_data_load("Weapon/assault_rifle.dat");
	type_data[WEAPON_Shotgun] = weapon_type_data_load("Weapon/shotgun.dat");
}

const Weapon_Type_Data* weapon_get_type_data(Weapon_Type type)
{
	return type_data[type];
}

Weapon_Instance weapon_instance_make(Weapon_Type type, u8 level)
{
	Weapon_Instance instance;
	instance.type = type;
	instance.attributes.level = level;

	return instance;
}