#pragma once
struct Mesh;
struct Material;
struct Convex_Shape;

struct Prop_Resource
{
	const Mesh* mesh;
	const Material* material;
	const Convex_Shape* shape;
};

Prop_Resource* prop_resource_load(const char* path);