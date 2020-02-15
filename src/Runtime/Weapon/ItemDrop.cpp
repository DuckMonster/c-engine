#include "ItemDrop.h"
#include "Runtime/Game/Scene.h"
#include "Runtime/Game/Game.h"

void item_drop_init(Item_Drop* drop, const Vec3& position, const Weapon_Instance& weapon)
{
	drop->position = position;
	drop->weapon = weapon;
}

void item_drop_free(Item_Drop* drop)
{
}

#if CLIENT
void item_drop_render(Item_Drop* drop, const Render_State& state)
{
	if (drop->hovered)
		scene_draw_sphere(drop->position, 0.5f, Color_White);
	else
		scene_draw_sphere(drop->position, 0.3f, Color_Gray);
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