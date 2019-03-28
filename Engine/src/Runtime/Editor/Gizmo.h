#pragma once
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"

struct Gizmo
{
	Mesh mesh;
	Material_Standard material;
};

extern Gizmo gizmo;

void gizmo_init();
void gizmo_update_and_render();