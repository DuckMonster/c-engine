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
#define MAX_DRAWABLES 256

struct Scene
{
#if CLIENT
	Drawable* floor;
	Camera camera;
#endif

	Unit units[MAX_UNITS];
	Sparse_List<Projectile> projectiles;

	Drawable drawables[MAX_DRAWABLES];
	bool drawable_enable[MAX_DRAWABLES];
};
extern Scene scene;

void scene_init();

Drawable* scene_make_drawable();
void scene_destroy(Drawable* drawable);

void scene_render(const Render_State& state);

#if CLIENT
Ray scene_mouse_ray();
Ray scene_screen_to_ray(Vec2 screen);
Vec2 scene_project_to_screen(const Vec3& position);
#endif