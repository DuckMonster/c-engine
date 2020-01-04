#pragma once
#include "Engine/Graphics/Mesh.h"
#include "Runtime/Render/Render.h"

struct Material;

enum Primitive_Type
{
	PRIM_Line
};

struct Primitive_Draw
{
	Primitive_Type type;
	void* ptr;

	float remove_time;
	Primitive_Draw* prev = nullptr;
	Primitive_Draw* next = nullptr;
};

struct Primitive_Line
{
	Vec3 from;
	Vec3 to;
};

struct Primitive_Manager
{
	Primitive_Draw* draw_list = nullptr;
	const Material* line_material;
	Mesh line_mesh;
};

void primitives_init(Primitive_Manager* manager);
void primitives_render(Primitive_Manager* manager, const Render_State& state);
void primitive_draw_line(Primitive_Manager* manager, const Vec3& from, const Vec3& to, float duration = 0.f);