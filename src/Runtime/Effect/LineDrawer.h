#pragma once
#include "Engine/Graphics/Mesh.h"
#include "Runtime/Render/Render.h"

struct Line_Segment
{
	Vec3 position;
	float time;
	float width;
};

struct Line_Drawer
{
	Vec3 position;
	Mesh mesh;

	Line_Segment segments[20];
	u32 num_segments = 0;
	float timer = 0.f;
	float lifetime = 0.f;
};

void line_drawer_init();
Line_Drawer* line_drawer_make();
void line_drawer_destroy(Line_Drawer* drawer);
void line_drawer_add_segment(Line_Drawer* drawer);

void line_drawer_render(const Render_State& state);