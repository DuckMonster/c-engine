#pragma once
#include "Core/Container/SparseList.h"
#include "Runtime/Game/HandleTypes.h"
#include "Engine/Debug/Primitive.h"
#include "Engine/Collision/CollisionTypes.h"
#include "ThingArray.h"

#define MAX_UNITS 256
#define MAX_WEAPONS 256
#define MAX_BULLETS 256
#define MAX_DRAWABLES 256
#define MAX_PROPS 256
#define MAX_ITEM_DROPS 256
#define MAX_BILLBOARDS 256
#define MAX_LINE_DRAWERS 256
#define MAX_HEALTH_BARS 256

struct Unit;
struct Weapon;
struct Weapon_Instance;
struct Bullet;
struct Bullet_Params;
struct Prop;
struct Item_Drop;
struct Drawable;
struct Billboard;
struct Line_Drawer;
struct Health_Bar;
struct Mesh;
struct Material;
struct Texture;
struct Sprite_Sheet;
struct Grass;

struct Scene
{
	Thing_Array<Unit> units;
	Thing_Array<Weapon> weapons;
	Thing_Array<Bullet> bullets;
	Thing_Array<Prop> props;
	Thing_Array<Item_Drop> drops;

#if CLIENT
	Thing_Array<Drawable> drawables;
	Thing_Array<Billboard> billboards;
	Thing_Array<Line_Drawer> line_drawers;
	Thing_Array<Health_Bar> health_bars;

	Grass* grass;
	Primitive_Manager primitive_manager;
#endif
};
extern Scene scene;

void scene_init();
void scene_update();

Unit* scene_make_unit(i32 id, const Vec3& position);
inline Unit* scene_make_unit(const Vec3& position) { scene_make_unit(-1, position); }
void scene_destroy_unit(Unit* unit);
u32 scene_get_free_unit_id();

Unit_Handle scene_unit_handle(Unit* unit);
Unit_Handle scene_unit_handle(u32 id);
Unit* scene_get_unit(const Unit_Handle& handle);

Weapon* scene_make_weapon(Unit* owner, const Weapon_Instance& instance);
void scene_destroy_weapon(Weapon* weapon);

Bullet* scene_make_bullet(u32 blt_index, const Unit_Handle& owner, const Bullet_Params& params);
void scene_destroy_bullet(Bullet* bullet);

Prop* scene_make_prop(const char* path);
void scene_destroy_prop(Prop* prop);

Item_Drop* scene_make_item_drop(const Thing_Handle& hndl, const Vec3& position, const Weapon_Instance& weapon);
void scene_destroy_item_drop(Item_Drop* drop);

#if CLIENT
Drawable* scene_make_drawable(const Mesh* mesh = nullptr, const Material* material = nullptr, const Texture* texture = nullptr);
void scene_destroy_drawable(Drawable* drawable);

Billboard* scene_make_billboard(const Sprite_Sheet* sheet);
void scene_destroy_billboard(Billboard* billboard);

Line_Drawer* scene_make_line_drawer(const Vec3& origin);
void scene_destroy_line_drawer(Line_Drawer* line_drawer);

Health_Bar* scene_make_health_bar();
void scene_destroy_health_bar(Health_Bar* bar);

// Drawing primitives
void scene_draw_line(const Vec3& from, const Vec3& to, const Vec4& color = Color_White, float duration = 0.f);
void scene_draw_point(const Vec3& position, const Vec4& color = Color_White, float duration = 0.f);
void scene_draw_sphere(const Vec3& origin, float radius, const Vec4& color = Color_White, float duration = 0.f);
void scene_draw_box(const Vec3& position, const Vec3& size, const Quat& orientation, const Vec4& color = Color_White, float duration = 0.f);
void scene_draw_convex_shape(const Convex_Shape* shape);

void scene_render(const Render_State& state);
#endif