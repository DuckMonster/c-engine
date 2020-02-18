#include "Player.h"
#include "Core/Input/Input.h"
#include "Engine/Collision/HitTest.h"
#include "Runtime/Game/Game.h"
#include "Runtime/Game/Scene.h"
#include "Runtime/Unit/Unit.h"
#include "Runtime/Weapon/ItemDrop.h"
#include "Runtime/Weapon/WeaponType.h"
#include "Runtime/Weapon/Weapon.h"
#include "Runtime/Weapon/Pistol.h"
#include "Runtime/Weapon/AssaultRifle.h"

#if CLIENT
void player_shooting_update_local(Player* player)
{
	if (game.is_editor)
		return;

	Unit* unit = scene_get_unit(player->controlled_unit);
	Player_Shooting& shooting = player->shooting;

	/* Calculate mouse position */
	Ray mouse_ray = game_mouse_ray();
	Plane ground_plane;
	ground_plane.normal = Vec3_Z;
	ground_plane.point = Vec3(0.f, 0.f, 0.5f);

	Hit_Result ground_hit = test_ray_plane(mouse_ray, ground_plane);
	shooting.aim_position = ground_hit.position;

	/* Update pickups */
	Item_Drop* drop = item_drop_get_closest(ground_hit.position, 1.f);
	if (drop != shooting.hovered_drop && shooting.hovered_drop)
		shooting.hovered_drop->hovered = false;

	shooting.hovered_drop = drop;
	if (drop)
	{
		drop->hovered = true;
		if (input_mouse_button_pressed(Mouse_Btn::Right))
		{
			channel_reset(player->channel);
			channel_write_u8(player->channel, PLAYEREV_Pickup_Weapon);
			channel_write_t(player->channel, drop->weapon);
			channel_broadcast(player->channel, true);

			unit_equip_weapon(unit, drop->weapon);
			scene_destroy_item_drop(drop);
			shooting.hovered_drop = nullptr;
		}
	}

	/* Shooting */
	if (unit->weapon != nullptr)
	{
		if (input_mouse_button_down(Mouse_Btn::Left))
			weapon_hold_trigger(unit->weapon, shooting.aim_position);
		if (input_mouse_button_released(Mouse_Btn::Left))
			weapon_release_trigger(unit->weapon, shooting.aim_position);
	}
}
#endif

void player_shooting_update(Player* player)
{
	Unit* unit = scene_get_unit(player->controlled_unit);
	if (unit == nullptr)
		return;

	if (unit_has_control(unit))
	{
#if CLIENT
		player_shooting_update_local(player);
#endif
	}

	unit->aim_direction = normalize_safe(player->shooting.aim_position - unit->position);
}