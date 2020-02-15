#include "AssaultRifle.h"

void assault_rifle_init(Assault_Rifle* rifle, Weapon* weapon, const Weapon_Attributes& attributes)
{
	rifle->weapon = weapon;
}

void assault_rifle_free(Assault_Rifle* rifle)
{
}

void assault_rifle_update(Assault_Rifle* rifle)
{
	if (rifle->trigger_held)
	{
		float fire_rate_delta = 1.f / assault_rifle_fire_rate;

		// OK to fire
		if (rifle->last_fire_time + fire_rate_delta < time_elapsed())
		{
			debug_log("Pew!");
			rifle->last_fire_time = time_elapsed();
		}

		rifle->trigger_held = false;
	}
}

void assault_rifle_fire(Assault_Rifle* rifle, const Vec2& target)
{
	rifle->trigger_held = true;
	rifle->fire_target = target;
}