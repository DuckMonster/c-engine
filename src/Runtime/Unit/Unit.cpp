#include "Unit.h"
#include "Engine/Render/Billboard.h"
#include "Runtime/Game/Scene.h"
#include "Runtime/Online/Channel.h"

namespace
{
}

void unit_event_proc(Channel* chnl)
{

}

Unit* unit_spawn(u32 id, const Vec2& position)
{
	Unit* unit = &scene.units[id];
	assert_msg(!unit->active, "Tried to spawn unit %d, but it's already active");

	unit->active = true;
	unit->id = id;
	unit->position = position;
	unit->target_position = position;

	unit->channel = channel_open("UNIT", id, unit_event_proc);
	unit->channel->user_ptr = unit;

#if CLIENT
	unit->billboard = billboard_load("Sprite/test_sheet.dat");
	unit->billboard->position = Vec3(position, 0.f);
	unit->billboard->tile_y = random_int(0, 3);
#endif

	return unit;
}

void unit_destroy(Unit* unit)
{
	assert_msg(unit->active, "Tried to destroy %d, but it's not active");

	channel_close(unit->channel);

#if CLIENT
	billboard_destroy(unit->billboard);
	unit->billboard = nullptr;
#endif

	*unit = Unit();
}

inline void unit_update(Unit* unit)
{
#if CLIENT
	unit->billboard->position = Vec3(unit->position, 0.f);
#endif

	Vec2 to_target = unit->target_position - unit->position;
	if (nearly_zero(to_target, unit->move_speed * time_delta()))
		return;

	to_target = normalize(to_target);
	unit->position += to_target * unit->move_speed * time_delta();
}

#if SERVER
void unit_move_to(Unit* unit, const Vec2& target)
{
}
#endif


void units_update()
{
	for(u32 i=0; i<MAX_UNITS; ++i)
	{
		if (!scene.units[i].active)
			continue;

		unit_update(scene.units + i);
	}
}