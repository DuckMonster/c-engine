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
#define MAX_UNITS 256
#define MAX_PROJECTILES 256
#define MAX_DRAWABLES 256
#define MAX_BILLBOARDS 256
#define MAX_LINE_DRAWERS 256
#define MAX_HEALTH_BARS 256

// Array wrapper that contains data and an enable-list
// Elements are never removed or added in memory, they are simply enabled or disabled
template<typename Type>
struct Thing_Array
{
	Type* data = nullptr;
	bool* enable = nullptr;
	u32 size = 0;

	// Either returns a pointer to the element, or nullptr if it isn't enabled
	inline Type* operator[](u32 index)
	{
		assert_msg(index < size, "Thing array index out of bounds");
		if (enable[index])
			return data + index;
		else
			return nullptr;
	}
	inline const Type* operator[](u32 index) const
	{
		assert_msg(index < size, "Thing array index out of bounds");
		if (enable[index])
			return data + index;
		else
			return nullptr;
	}
};

template<typename Type>
void thing_array_init(Thing_Array<Type>* array, u32 size)
{
	array->data = new Type[size];
	array->enable = new bool[size];
	mem_zero(array->enable, size);

	array->size = size;
}

template<typename Type>
Type* thing_add(Thing_Array<Type>* array)
{
	for(u32 i=0; i<array->size; ++i)
	{
		if (!array->enable[i])
		{
			array->enable[i] = true;
			return array->data + i;
		}
	}

	error("Ran out of space in thing array");
	return nullptr;
}

template<typename Type>
Type* thing_add_at(Thing_Array<Type>* array, u32 index)
{
	assert_msg(index < array->size, "Tried too add specific index, but index was out of bounds");
	assert_msg(!array->enable[index], "Tried to add specific index in thing array, but that index is full");
	array->enable[index] = true;
	return array->data + index;
}

template<typename Type>
void thing_remove(Thing_Array<Type>* array, Type* entry)
{
	u32 index = entry - array->data;
	assert_msg(index < array->size, "Tried to remove entry from thing array that is not created from the same array");
	assert_msg(array->enable[index], "Tried to remove entry from thing array that is not enabled");

	array->enable[index] = false;
	*entry = Type();
}

template<typename Type>
bool _thing_iterate(Thing_Array<Type>* array, Type*& it)
{
	if (it == nullptr)
		return false;

	u32 index = it - array->data;
	while(index < array->size)
	{
		if (array->enable[index])
		{
			it = array->data + index;
			return true;
		}

		index++;
	}

	return false;
}

#define THINGS_FOREACH(array) for(auto it = (array)->data; _thing_iterate(array, it); it++)

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

Projectile* scene_make_projectile(Unit* owner, u32 proj_id, const Vec2& origin, const Vec2& direction);
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