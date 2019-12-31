#pragma once
#include "Engine/Graphics/Mesh.h"
#include "Runtime/Render/Render.h"

struct Material;

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
	const Material* material;

	Line_Segment segments[20];
	u32 num_segments = 0;
	float timer = 0.f;
	float lifetime = 0.f;

	bool should_destroy = false;
};

#if CLIENT
void line_drawer_init(Line_Drawer* drawer, const Vec3& origin);
void line_drawer_free(Line_Drawer* drawer);
void line_drawer_add_segment(Line_Drawer* drawer);

void line_drawer_render(Line_Drawer* drawer, const Render_State& state);
#endif