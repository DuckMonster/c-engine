#pragma once
struct Billboard;
struct Channel;
struct Online_User;
struct Health_Bar;

struct Unit
{
	u32 id;
	Vec2 position;
	Vec2 target_position;
	Vec2 aim_direction;
	float move_speed = 6.f;

	Channel* channel = nullptr;

	float health;
	float health_max = 5.f;

#if CLIENT
	Billboard* billboard = nullptr;
	Billboard* gun_billboard = nullptr;
	Health_Bar* health_bar = nullptr;
	bool is_local = false;
#endif

#if SERVER
	Online_User* owner = nullptr;
	float walk_timer = 2.f;
	Vec2 ai_walk_target;
#endif
};

void unit_init(Unit*unit, u32 id, const Vec2& position);
void unit_free(Unit* unit);
void unit_update(Unit* unit);

void unit_move_towards(Unit* unit, const Vec2& target);
void unit_move_direction(Unit* unit, const Vec2& direction);
void unit_shoot(Unit* unit, const Vec2& target);
void unit_hit(Unit* unit, const Vec2& impulse);
bool unit_has_control(Unit* unit);
