#pragma once
#include "Core/Container/SparseList.h"
#include "Core/Math/Ray.h"
#include "Runtime/Render/Drawable.h"
#include "Runtime/Render/Billboard.h"
#include "Runtime/Render/HealthBar.h"
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
#define MAX_HEALTH_BARS 256

struct Scene
{
#if CLIENT
	Drawable* floor;
	Camera camera;
#endif

	Unit units[MAX_UNITS];
	bool unit_enable[MAX_UNITS];

	Sparse_List<Projectile> projectiles;
	bool projectile_enable[MAX_PROJECTILES];

	Drawable drawables[MAX_DRAWABLES];
	bool drawable_enable[MAX_DRAWABLES];

	Billboard billboards[MAX_BILLBOARDS];
	bool billboard_enable[MAX_BILLBOARDS];

	Line_Drawer line_drawers[MAX_LINE_DRAWERS];
	bool line_drawer_enable[MAX_LINE_DRAWERS];

	Health_Bar health_bars[MAX_HEALTH_BARS];
	bool health_bar_enable[MAX_HEALTH_BARS];
};
extern Scene scene;

void scene_init();
void scene_update();

Unit* scene_make_unit(i32 id, const Vec2& position);
inline Unit* scene_make_unit(const Vec2& position) { scene_make_unit(-1, position); }
void scene_destroy_unit(Unit* unit);

#if CLIENT
Drawable* scene_make_drawable(const Mesh* mesh = nullptr, const Material* material = nullptr, const Texture* texture = nullptr);
void scene_destroy_drawable(Drawable* drawable);

Billboard* scene_make_billboard(const Sprite_Sheet* sheet);
void scene_destroy_billboard(Billboard* billboard);

Line_Drawer* scene_make_line_drawer(const Vec3& origin);
void scene_destroy_line_drawer(Line_Drawer* line_drawer);

Health_Bar* scene_make_health_bar();
void scene_destroy_health_bar(Health_Bar* bar);

void scene_render(const Render_State& state);

Ray scene_mouse_ray();
Ray scene_screen_to_ray(Vec2 screen);
Vec2 scene_project_to_screen(const Vec3& position);
#endif