#pragma once
struct Billboard;
struct Channel;
struct Online_User;

struct Unit
{
	bool active = false;
	u32 id;
	Vec2 position;
	Vec2 target_position;
	Vec2 aim_direction;
	float move_speed = 6.f;

	Channel* channel = nullptr;

#if CLIENT
	Billboard* billboard = nullptr;
	Billboard* gun_billboard = nullptr;
	bool is_local = false;
#endif

#if SERVER
	Online_User* owner = nullptr;
	float walk_timer = 2.f;
	Vec2 ai_walk_target;
#endif
};

Unit* unit_spawn(u32 id, const Vec2& position);
void unit_destroy(Unit* unit);

void unit_move_towards(Unit* unit, const Vec2& target);
void unit_move_direction(Unit* unit, const Vec2& direction);
void unit_shoot(Unit* unit, const Vec2& target);
void unit_hit(Unit* unit, const Vec2& impulse);
bool unit_has_control(Unit* unit);

void units_update();