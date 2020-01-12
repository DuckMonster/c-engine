#include "SceneQuery.h"
#include "Scene.h"

#if CLIENT
static void scene_draw_line_query_result(const Line_Trace& line, const Hit_Result& hit, float duration)
{
	if (hit.has_hit)
	{
		scene_draw_line(line.from, hit.position, Color_Red, duration);
		scene_draw_point(hit.position, Color_Red, duration);
		scene_draw_line(hit.position, hit.position + hit.normal, Color_Red, duration);
	}
	else
	{
		scene_draw_line(line.from, line.to, Color_Green, duration);
	}
}
#endif

Scene_Query_Result scene_query_line(const Line_Trace& line, const Scene_Query_Params& params)
{
	// Query all units
	Scene_Query_Result result;
	result.hit.time = BIG_NUMBER;

	/* UNITS */
	if (params.mask & QUERY_Unit)
	{
		THINGS_FOREACH(&scene.units)
		{
			Unit* unit = it;
			if (unit == params.ignore_unit)
				continue;

			Sphere unit_sphere;
			unit_sphere.origin = Vec3(unit->position, 0.5f);
			unit_sphere.radius = 0.5f;

			Hit_Result hit = test_line_trace_sphere(line, unit_sphere);
			if (hit.has_hit && hit.time < result.hit.time)
			{
				result.hit = hit;
				result.unit = unit;
			}
		}
	}

	/* PROPS */
	if (params.mask & QUERY_Props)
	{
		Hit_Result hit = test_line_trace_sphere(line, scene.sphere);
		if (hit.has_hit && hit.time < result.hit.time)
		{
			result.hit = hit;
			result.unit = nullptr;
		}

		hit = test_line_trace_aligned_box(line, scene.aligned_box);
		if (hit.has_hit && hit.time < result.hit.time)
		{
			result.hit = hit;
			result.unit = nullptr;
		}

		hit = test_line_trace_box(line, scene.box);
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

	// Do a bit of pullback at the to, to avoid movement and such being inside of geometry
	if (result.hit.has_hit)
		result.hit.time = max(result.hit.time - 0.08f, 0.f);

	return result;
}

bool scene_query_vision(const Vec2& from, const Vec2& to)
{
	Line_Trace line;
	line.from = Vec3(from, 0.5f);
	line.to = Vec3(to, 0.5f);

	Scene_Query_Params params;
	params.mask = ~QUERY_Unit;

	Scene_Query_Result result = scene_query_line(line, params);
	return !result.hit.has_hit;
}