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
#include "ThingArray.h"

#define MAX_PLAYERS 10
#define MAX_UNITS 256
#define MAX_PROJECTILES 256
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

	Thing_Array<Unit> units;
	Thing_Array<Projectile> projectiles;
	Thing_Array<Drawable> drawables;
	Thing_Array<Billboard> billboards;
	Thing_Array<Line_Drawer> line_drawers;
	Thing_Array<Health_Bar> health_bars;
};
extern Scene scene;

void scene_init();
void scene_update();

Unit* scene_make_unit(i32 id, const Vec2& position);
inline Unit* scene_make_unit(const Vec2& position) { scene_make_unit(-1, position); }
void scene_destroy_unit(Unit* unit);
u32 scene_get_free_unit_id();

Unit_Handle scene_unit_handle(Unit* unit);
Unit_Handle scene_unit_handle(u32 id);
Unit* scene_get_unit(const Unit_Handle& handle);

Projectile* scene_make_projectile(const Unit_Handle& owner, u32 proj_id, const Vec2& origin, const Vec2& direction);
void scene_destroy_projectile(Projectile* projectile);

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