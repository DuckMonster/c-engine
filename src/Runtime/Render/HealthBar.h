#pragma once
#include "Engine/Graphics/Mesh.h"
#include "Render.h"
struct Material;
struct Unit;

#define HEALTH_BAR_HEIGHT 0.15f

struct Health_Bar
{
	Mesh mesh;
	const Material* material;

	Vec3 position;
	float health_percent = 1.f;
	float previous_health = 1.f;
};

void health_bar_init(Health_Bar* bar);
void health_bar_free(Health_Bar* bar);
void health_bar_update(Health_Bar* bar);
void health_bar_render(Health_Bar* bar, const Render_State& state);
