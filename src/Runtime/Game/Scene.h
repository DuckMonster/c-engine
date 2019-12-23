#pragma once
#include "Core/Container/SparseList.h"
#include "Core/Math/Ray.h"
#include "Engine/Render/Drawable.h"
#include "Runtime/Unit/Unit.h"
#include "Runtime/Game/Camera.h"
#include "Runtime/Game/Projectile.h"

#define MAX_PLAYERS 10
#define MAX_UNITS 10
#define MAX_PROJECTILES 100

struct Scene
{
#if CLIENT
	Drawable* floor;
	Camera camera;
#endif

	Unit units[MAX_UNITS];
	Sparse_List<Projectile> projectiles;
};
extern Scene scene;

void scene_init();

#if CLIENT
Ray scene_mouse_ray();
Ray scene_screen_to_ray(Vec2 screen);
Vec2 scene_project_to_screen(const Vec3& position);
#endif