#include "ItemDrop.h"
#include "Engine/Graphics/SpriteSheet.h"
#include "Runtime/Game/Scene.h"
#include "Runtime/Game/Game.h"
#include "Runtime/Render/Billboard.h"

void item_drop_init(Item_Drop* drop, const Vec3& position, const Weapon_Instance& weapon)
{
	drop->position = position;
	drop->weapon = weapon;

#if CLIENT
	const Weapon_Type_Data* weapon_data = weapon_get_type_data(weapon.type);

	drop->billboard = scene_make_billboard(sprite_sheet_load("Sprite/weapon_sheet.dat"));
	drop->billboard->cast_shadow = false;
	drop->billboard->position = position + Vec3(0.f, 0.f, 1.f);
	drop->billboard->alpha = 0.f;
	drop->billboard->tile_x = weapon_data->tile_x;
	drop->billboard->tile_y = weapon_data->tile_y;
	drop->billboard->rotation_angle = -radians(20.f);
#endif
}

void item_drop_free(Item_Drop* drop)
{
#if CLIENT
	scene_destroy_billboard(drop->billboard);
#endif
}

#if CLIENT
void item_drop_render(Item_Drop* drop, const Render_State& state)
{
	float target_alpha = drop->hovered ? 1.f : 0.f;
	Vec3 target_offset = Vec3(0.f, 0.f, drop->hovered ? 1.f : 0.3f);
	drop->billboard_offset = lerp(drop->billboard_offset, target_offset, 12.f * time_delta());

	drop->billboard->alpha = lerp(drop->billboard->alpha, target_alpha, 12.f * time_delta());
	drop->billboard->position = drop->position + drop->billboard_offset;

	if (drop->hovered)
		scene_draw_sphere(drop->position, 0.3f, Color_White);
	else
		scene_draw_sphere(drop->position, 0.15f, Color_Gray);
}
#endif

Item_Drop* item_drop_get_closest(const Vec3& position, float max_radius)
{
	Item_Drop* closest = nullptr;
	float closest_dist_sqrd;
	float max_radius_sqrd = square(max_radius);

	THINGS_FOREACH(&scene.drops)
	{
		float dist_sqrd = distance_sqrd(it->position, position);

		if (dist_sqrd > max_radius_sqrd)
			continue;

		if (closest == nullptr || dist_sqrd < closest_dist_sqrd)
		{
			closest = it;
			closest_dist_sqrd = dist_sqrd;
		}
	}

	return closest;
}