#pragma once
struct Billboard;
struct Channel;
struct Online_User;
struct Health_Bar;

const float unit_impact_drag = 8.5f;
const float unit_hit_duration = 0.15f;

struct Unit
{
	u32 id;
	Vec2 position;
	Vec2 target_position;
	Vec2 aim_direction;
	Vec2 impact_velocity;
	float move_speed = 6.f;

	Channel* channel = nullptr;

	float health;
	float health_max = 5.f;

	Unit* target = nullptr;

#if CLIENT
	Billboard* billboard = nullptr;
	Billboard* gun_billboard = nullptr;
	Health_Bar* health_bar = nullptr;
	bool is_local = false;

	float hit_timer = 0.f;
#endif

#if SERVER
	Online_User* owner = nullptr;
	float walk_timer = 2.f;
	Vec2 ai_walk_target;

	float ai_shoot_timer = 0.f;
#endif
};

void unit_init(Unit*unit, u32 id, const Vec2& position);
void unit_free(Unit* unit);
void unit_update(Unit* unit);

#if SERVER
void unit_serialize_to(Unit* unit, Online_User* user);
#endif

void unit_move_towards(Unit* unit, const Vec2& target);
void unit_move_direction(Unit* unit, const Vec2& direction);
void unit_shoot(Unit* unit, const Vec2& target);
void unit_hit(Unit* unit, const Vec2& impulse);
bool unit_has_control(Unit* unit);
