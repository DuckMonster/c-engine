#include "SceneQuery.h"
#include "Scene.h"
#include "Core/Debug/Profiling.h"
#include "Runtime/Unit/Unit.h"
#include "Runtime/Prop/Prop.h"

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

Scene_Query_Result select_result(const Scene_Query_Result& a, const Scene_Query_Result& b)
{
	if (!a.hit.has_hit)
		return b;

	if (!b.hit.has_hit)
		return a;

	return a.hit.time < b.hit.time ? a : b;
}

Scene_Query_Result scene_query_line(const Line_Trace& line, const Scene_Query_Params& params)
{
	pix_event_scope("SceneQueryLine");

	// Query all units
	Scene_Query_Result result;
	result.hit.time = BIG_NUMBER;

	/* UNITS */
	if (params.mask & QUERY_Unit)
	{
		Scene_Query_Result unit_result;

		THINGS_FOREACH(&scene.units)
		{
			Unit* unit = it;
			if (unit == params.ignore_unit)
				continue;

			if (!unit_is_alive(unit))
				continue;

			Sphere unit_sphere;
			unit_sphere.origin = unit->position + unit_center_offset;
			unit_sphere.radius = 0.5f;

			unit_result.unit = unit;
			unit_result.hit = test_line_trace_sphere(line, unit_sphere);

			result = select_result(result, unit_result);
		}
	}

	/* PROPS */
	if (params.mask & QUERY_Props)
	{
		Scene_Query_Result prop_result;

		THINGS_FOREACH(&scene.props)
		{
			prop_result.prop = it;
			prop_result.hit = test_line_trace_shape(line, &it->shape);

			result = select_result(result, prop_result);
		}
	}

	/* ENVIRONMENT */
	if (params.mask & QUERY_Environment)
	{
		Scene_Query_Result env_result;

		Plane floor_plane = plane_make(Vec3(0.f, 0.f, -0.05f), Vec3_Z);
		env_result.hit = test_line_trace_plane(line, floor_plane);

		result = select_result(result, env_result);
	}

	/* DEBUG */
#if CLIENT
	if (params.debug_render)
	{
		Aligned_Box box;
		box.position = Vec3(0.f, 20.f, 0.f);
		box.size = Vec3(4.f, 3.f, 2.f);
		scene_draw_box(box.position, box.size, Quat_Identity);

		Scene_Query_Result debug_result;
		debug_result.hit = test_line_trace_aligned_box(line, box);

		result = select_result(result, debug_result);
	}
#endif

#if CLIENT
	if (params.debug_render)
		scene_draw_line_query_result(line, result.hit, params.debug_render_duration);
#endif

	// Do a bit of pullback at the to, to avoid movement and such being inside of geometry
	if (result.hit.has_hit)
	{
		float pullback_length = 0.05f;
		float pullback_time = pullback_length / distance(line.from, line.to);
		result.hit.time = max(result.hit.time - pullback_time, 0.f);
		result.hit.position = lerp(line.from, line.to, result.hit.time);
	}

	return result;
}

bool scene_query_vision(const Vec3& from, const Vec3& to)
{
	Line_Trace line;
	line.from = from + Vec3(0.f, 0.f, 0.5f);
	line.to = to + Vec3(0.f, 0.f, 0.5f);

	Scene_Query_Params params;
	params.mask = ~QUERY_Unit;

	Scene_Query_Result result = scene_query_line(line, params);
	return !result.hit.has_hit;
}