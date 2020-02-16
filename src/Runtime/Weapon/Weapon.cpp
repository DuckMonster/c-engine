#include "Weapon.h"
#include "Core/Input/Input.h"
#include "Engine/Graphics/SpriteSheet.h"
#include "Runtime/Game/Game.h"
#include "Runtime/Game/Scene.h"
#include "Runtime/Unit/Unit.h"
#include "Runtime/Render/Billboard.h"
#include "Runtime/Fx/Fx.h"
#include "Runtime/Weapon/WeaponType.h"
#include "Pistol.h"
#include "AssaultRifle.h"

void weapon_free_type_ptr(void* ptr, const Weapon_Instance& instance)
{
}

void weapon_init(Weapon* weapon, Unit* owner, const Weapon_Instance& instance)
{
	weapon->owner = scene_unit_handle(owner);
	weapon->position = owner->position;
	weapon->type = instance.type;

	switch(weapon->type)
	{
		case WEAPON_Pistol:
		{
			Pistol* pistol = new Pistol();
			pistol_init(pistol, weapon, instance.attributes);

			weapon->weapon_type_ptr = pistol;
			break;
		}

		case WEAPON_AssaultRifle:
		{
			Assault_Rifle* rifle = new Assault_Rifle();
			assault_rifle_init(rifle, weapon, instance.attributes);

			weapon->weapon_type_ptr = rifle;
			break;
		}
	}

#if CLIENT
	const Weapon_Type_Data* type_data = weapon_get_type_data(instance.type);

	weapon->billboard = scene_make_billboard(sprite_sheet_load("Sprite/weapon_sheet.dat"));
	weapon->billboard->position = owner->position;
	weapon->billboard->rotation_type = ROTATION_World_Direction;
	weapon->billboard->tile_x = type_data->tile_x;
	weapon->billboard->tile_y = type_data->tile_y;
#endif
}

void weapon_free(Weapon* weapon)
{
#if CLIENT
	scene_destroy_billboard(weapon->billboard);
#endif

	void* type_ptr = weapon->weapon_type_ptr;

	switch(weapon->type)
	{
		case WEAPON_Pistol:
			pistol_free((Pistol*)type_ptr);
			break;

		case WEAPON_AssaultRifle:
			assault_rifle_free((Assault_Rifle*)type_ptr);
			break;
	}

	delete type_ptr;
}

void weapon_update(Weapon* weapon)
{
	Unit* owner = scene_get_unit(weapon->owner);
	assert(owner);

	Vec2 target = owner->position + owner->aim_direction * weapon_hold_distance;
	weapon->position = lerp(weapon->position, target, weapon_interp_speed * time_delta());

	switch(weapon->type)
	{
		case WEAPON_AssaultRifle:
			assault_rifle_update((Assault_Rifle*)weapon->weapon_type_ptr);
			break;
	}

#if CLIENT
	// Move billboard
	weapon->billboard->position = Vec3(weapon->position, 0.5f);

	/* Shoot animation stuff */
	weapon->offset += weapon->offset_velocity * time_delta();
	weapon->offset_velocity -= weapon->offset * weapon_offset_acceleration * time_delta();
	weapon->offset_velocity -= weapon->offset_velocity * weapon_offset_drag * time_delta();

	weapon->angle_offset += weapon->angle_offset_velocity * time_delta();
	weapon->angle_offset_velocity -= weapon->angle_offset * weapon_angle_offset_acceleration * time_delta();
	weapon->angle_offset_velocity -= weapon->angle_offset_velocity * weapon_angle_offset_drag * time_delta();

	weapon->billboard->position += Vec3(weapon->offset, 0.f);

	// Rotation
	Quat weapon_quat = quat_from_x(Vec3(owner->aim_direction, 0.f));
	Quat recoil_quat = angle_axis(weapon->angle_offset, -Vec3_Y);

	Vec3 aim_forward = quat_x(weapon_quat * recoil_quat);
	weapon->billboard->rotation_direction = aim_forward;
#endif
}

#if CLIENT
void weapon_reset_offset(Weapon* weapon)
{
	weapon->offset = Vec2();
	weapon->angle_offset = 0.f;
}

void weapon_add_velocity(Weapon* weapon, const Vec2& linear_velocity, float angular_velocity)
{
	weapon->offset_velocity += linear_velocity;
	weapon->angle_offset_velocity += angular_velocity;
}
#endif