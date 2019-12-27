#pragma once
#include "Core/Container/SparseList.h"
#include "Core/Math/Ray.h"
#include "Runtime/Render/Drawable.h"
#include "Runtime/Render/Billboard.h"
#include "Runtime/Effect/LineDrawer.h"
#include "Runtime/Unit/Unit.h"
#include "Runtime/Game/Camera.h"
#include "Runtime/Game/Projectile.h"

#define MAX_PLAYERS 10
#define MAX_UNITS 10
#define MAX_PROJECTILES 100
#define MAX_DRAWABLES 256
#define MAX_BILLBOARDS 256
#define MAX_LINE_DRAWERS 256

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

	Billboard billboards[MAX_BILLBOARDS];
	bool billboard_enable[MAX_BILLBOARDS];

	Line_Drawer line_drawers[MAX_LINE_DRAWERS];
	bool line_drawer_enable[MAX_LINE_DRAWERS];
};
extern Scene scene;

void scene_init();

Drawable* scene_make_drawable();
void scene_destroy_drawable(Drawable* drawable);

Billboard* scene_make_billboard();
void scene_destroy_billboard(Billboard* billboard);

Line_Drawer* scene_make_line_drawer();
void scene_destroy_line_drawer(Line_Drawer* line_drawer);

#if CLIENT
void scene_render(const Render_State& state);

Ray scene_mouse_ray();
Ray scene_screen_to_ray(Vec2 screen);
Vec2 scene_project_to_screen(const Vec3& position);
#endif