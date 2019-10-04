#pragma once
struct Billboard;
struct Channel;

struct Unit
{
	bool active = false;
	u32 id;
	Vec2 position;
	Vec2 target_position;
	float move_speed = 4.f;

	Channel* channel = nullptr;

#if CLIENT
	Billboard* billboard = nullptr;
#endif

#if SERVER
	float walk_timer = 2.f;
#endif
};

Unit* unit_spawn(u32 id, const Vec2& position);
void unit_destroy(Unit* unit);

#if SERVER
void unit_move_to(Unit* unit, const Vec2& target);
#endif

void units_update();