#pragma once
#include "Engine/Graphics/Mesh.h"
#include "Runtime/Render/Render.h"
struct Material;

struct Grass
{
	Mesh mesh;
	const Material* material = nullptr;
};

void grass_init(Grass* grass);
void grass_update(Grass* grass);
void grass_render(Grass* grass, const Render_State& state);