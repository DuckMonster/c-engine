#pragma once
#include "Engine/Collision/HitTest.h"
struct Unit;

enum Scene_Query_Mask
{
	QUERY_Unit	= 1 << 0,
	QUERY_Props	= 1 << 1,
	QUERY_All = ~0
};

struct Scene_Query_Params
{
	Unit* ignore_unit = nullptr;
	u32 mask = QUERY_All;

	bool debug_render = false;
	float debug_render_duration = 0.f;
};

struct Scene_Query_Result
{
	Hit_Result hit;
	Unit* unit = nullptr;
};

Scene_Query_Result scene_query_line(const Line_Trace& line, const Scene_Query_Params& params = Scene_Query_Params());
// Will only query props at unit eye-height
bool scene_query_vision(const Vec2& from, const Vec2& to);