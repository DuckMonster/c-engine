#include "SceneQuery.h"
#include "Scene.h"

#if CLIENT
void scene_draw_line_query_result(const Line& line, const Hit_Result& hit, float duration)
{
	if (hit.has_hit)
	{
		scene_draw_line(line.start, hit.position, Color_Red, duration);
		scene_draw_point(hit.position, Color_Red, duration);
		scene_draw_line(hit.position, hit.position + hit.normal, Color_Red, duration);
	}
	else
	{
		scene_draw_line(line.start, line.end, Color_Green, duration);
	}
}
#endif

Scene_Query_Result scene_query_line(const Line& line, const Scene_Query_Params& params)
{
	// Query all units
	Scene_Query_Result result;
	result.hit.time = BIG_NUMBER;

	THINGS_FOREACH(&scene.units)
	{
		Unit* unit = it;
		if (unit == params.ignore_unit)
			continue;

		Sphere unit_sphere;
		unit_sphere.origin = Vec3(unit->position, 0.5f);
		unit_sphere.radius = 0.5f;

		Hit_Result hit = test_line_sphere(line, unit_sphere);
		if (hit.has_hit && hit.time < result.hit.time)
		{
			result.hit = hit;
			result.unit = unit;
		}
	}

	// Query obstacle
	{
		Hit_Result hit = test_line_sphere(line, scene.obstacle);
		if (hit.has_hit && hit.time < result.hit.time)
		{
			result.hit = hit;
			result.unit = nullptr;
		}
	}

#if CLIENT
	if (params.debug_render)
		scene_draw_line_query_result(line, result.hit, params.debug_render_duration);
#endif

	return result;
}