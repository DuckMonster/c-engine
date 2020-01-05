#include "SceneQuery.h"
#include "Scene.h"

Scene_Query_Result scene_query_line(const Line& line)
{
	Scene_Query_Result result;

	// Query all units
	THINGS_FOREACH(&scene.units)
	{
		Unit* unit = it;

		Sphere unit_sphere;
		unit_sphere.origin = Vec3(unit->position, 0.5f);
		unit_sphere.radius = 0.5f;

		result.hit = test_line_sphere(line, unit_sphere);
		if (result.hit.has_hit)
		{
			result.unit = unit;
			return result;
		}
	}

	return result;
}