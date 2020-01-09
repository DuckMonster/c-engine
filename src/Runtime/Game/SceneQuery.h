#pragma once
#include "Engine/Collision/HitTest.h"
struct Unit;

struct Scene_Query_Params
{
	Unit* ignore_unit = nullptr;
	bool debug_render = false;
	float debug_render_duration = 0.f;
};

struct Scene_Query_Result
{
	Hit_Result hit;
	Unit* unit = nullptr;
};

Scene_Query_Result scene_query_line(const Line_Trace& line, const Scene_Query_Params& params = Scene_Query_Params());