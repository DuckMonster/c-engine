#pragma once
#include "Engine/Graphics/Mesh.h"
#include "Runtime/Render/Render.h"

struct Material;

enum Primitive_Type
{
	PRIM_Line,
	PRIM_Point,
	PRIM_Sphere,
	PRIM_Box,
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
	Vec4 color;
};

struct Primitive_Point
{
	Vec3 position;
	Vec4 color;
};

struct Primitive_Sphere
{
	Vec3 origin;
	float radius;
	Vec4 color;
};

struct Primitive_Box
{
	Vec3 position;
	Vec3 size;
	Quat orientation;
	Vec4 color;
};

struct Primitive_Manager
{
	Primitive_Draw* draw_list = nullptr;

	// Line stuff
	const Material* line_material;
	Mesh line_mesh;

	// Point stuff
	const Material* point_material;
	Mesh point_mesh;

	// This material is used by any primitive represented by a mesh
	const Material* mesh_material;

	// Sphere stuff
	Mesh sphere_mesh;

	// Box stuff
	Mesh box_mesh;
};

void primitives_init(Primitive_Manager* manager);
void primitives_render(Primitive_Manager* manager, const Render_State& state);
void primitive_draw_line(Primitive_Manager* manager, const Vec3& from, const Vec3& to, const Vec4& color, float duration);
void primitive_draw_point(Primitive_Manager* manager, const Vec3& position, const Vec4& color, float duration);
void primitive_draw_sphere(Primitive_Manager* manager, const Vec3& origin, float radius, const Vec4& color, float duration);
void primitive_draw_box(Primitive_Manager* manager, const Vec3& position, const Vec3& size, const Quat& orientation, const Vec4& color, float duration);