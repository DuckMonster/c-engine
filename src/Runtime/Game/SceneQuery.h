#pragma once
#include "Engine/Collision/HitTest.h"
struct Unit;

struct Scene_Query_Result
{
	Hit_Result hit;
	Unit* unit = nullptr;
};

Scene_Query_Result scene_query_line(const Line& line);