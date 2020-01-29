#pragma once
struct Mesh;
struct Material;

struct Prop_Resource
{
	const Mesh* mesh;
	const Material* material;
};

Prop_Resource* prop_resource_load(const char* path);