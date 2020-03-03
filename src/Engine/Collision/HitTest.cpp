#include "HitTest.h"
#include "Core/Debug/Profiling.h"
#include "Runtime/Game/Scene.h"

Hit_Result hit_make(const Vec3& position, const Vec3& normal, float time)
{
	Hit_Result hit;
	hit.has_hit = true;
	hit.position = position;
	hit.normal = normal;
	hit.time = time;
	hit.started_penetrating = false;
	hit.penetration_depth = 0.f;

	return hit;
}
Hit_Result hit_make_depen(const Vec3& position, const Vec3& normal, float penetration_depth)
{
	Hit_Result hit;
	hit.has_hit = true;
	hit.position = position;
	hit.normal = normal;
	hit.time = 0.f;
	hit.started_penetrating = true;
	hit.penetration_depth = penetration_depth;

	return hit;
}

Hit_Result test_ray_plane(const Ray& ray, const Plane& plane)
{
	// Get the height of the ray from the plane
	Vec3 plane_to_ray = ray.origin - plane.point;
	float ray_height = dot(plane_to_ray, plane.normal);

	// Get the angle of the ray compared to the normal
	float ray_angle = dot(ray.direction, -plane.normal);

	// Paralell check, they will never intersect
	if (is_nearly_zero(ray_angle))
		return Hit_Result();

	// Time to impact with plane
	float impact_time = ray_height / ray_angle;

	// Backwards check, we only care about forward intersections
	if (impact_time < 0.f)
		return Hit_Result();

	return hit_make(ray.origin + ray.direction * impact_time, plane.normal * sign(ray_height), impact_time);
}

Hit_Result test_ray_sphere(const Ray& ray, const Sphere& sphere)
{
	float radius_sqrd = square(sphere.radius);

	Vec3 ray_to_sphere = sphere.origin - ray.origin;
	if (length_sqrd(ray_to_sphere) < radius_sqrd)
	{
		// Ray origin is inside the sphere, we have a from-penetrating
		float depth = sphere.radius - length(-ray_to_sphere);
		Vec3 normal = normalize(-ray_to_sphere);

		return hit_make_depen(ray.origin, normal, depth);
	}

	float closest_point_time = dot(ray_to_sphere, ray.direction);

	// Dont intersect if the closest point is behind us
	if (closest_point_time < 0.f)
		return Hit_Result();

	Vec3 closest_point = ray.origin + ray.direction * closest_point_time;
	float distance_to_sphere_sqrd = length_sqrd(closest_point - sphere.origin);

	// If the closest point is further away than the radius, we're clearly not intersecting
	if (distance_to_sphere_sqrd > radius_sqrd)
		return Hit_Result();

	float penetration_depth = sqrt(radius_sqrd - distance_to_sphere_sqrd);
	float intersection_time = closest_point_time - penetration_depth;
	Vec3 intersection_point = ray.origin + ray.direction * intersection_time;
	Vec3 intersection_normal = normalize(intersection_point - sphere.origin);

	return hit_make(intersection_point, intersection_normal, intersection_time);
}

Hit_Result test_ray_aligned_box(const Ray& ray, const Aligned_Box& box)
{
	Vec3 box_half_size = box.size * 0.5f;
	Vec3 box_min = box.position - box_half_size;
	Vec3 box_max = box.position + box_half_size;

	float x_min_t = (box_min.x - ray.origin.x) / ray.direction.x;
	float x_max_t = (box_max.x - ray.origin.x) / ray.direction.x;

	float x_entry_t = min(x_min_t, x_max_t);
	float x_exit_t = max(x_min_t, x_max_t);

	float y_min_t = (box_min.y - ray.origin.y) / ray.direction.y;
	float y_max_t = (box_max.y - ray.origin.y) / ray.direction.y;

	float y_exit_t = max(y_min_t, y_max_t);
	float y_entry_t = min(y_min_t, y_max_t);

	float z_min_t = (box_min.z - ray.origin.z) / ray.direction.z;
	float z_max_t = (box_max.z - ray.origin.z) / ray.direction.z;

	float z_entry_t = min(z_min_t, z_max_t);
	float z_exit_t = max(z_min_t, z_max_t);

	float max_entry = max(x_entry_t, max(y_entry_t, z_entry_t));
	float min_exit = min(x_exit_t, min(y_exit_t, z_exit_t));
	if (max_entry > min_exit)
		return Hit_Result();

	if (max_entry < 0.f)
		return hit_make_depen(ray.origin, Vec3_Zero, 0.f);

	Vec3 normal = Vec3_X;
	Vec3 intersection_point = (ray.origin + ray.direction * max_entry) - box.position;
	intersection_point = intersection_point / box.size;

	if (abs(intersection_point.x) > abs(intersection_point.y) &&
		abs(intersection_point.x) > abs(intersection_point.z))
		normal = Vec3_X * sign(intersection_point.x);
	else if (abs(intersection_point.y) > abs(intersection_point.z))
		normal = Vec3_Y * sign(intersection_point.y);
	else
		normal = Vec3_Z * sign(intersection_point.z);

	return hit_make(ray.origin + ray.direction * max_entry, normal, max_entry);
}

Hit_Result test_ray_box(const Ray& ray, const Box& box)
{
	// First we want to transform the ray into the box's local space, so that we can do 
	//	an axis-aligned test.
	Mat4 box_matrix = mat_position_rotation(box.position, box.orientation);
	Mat4 box_matrix_inv = inverse(box_matrix);

	Aligned_Box aligned_box;
	aligned_box.position = Vec3_Zero;
	aligned_box.size = box.size;

	Ray local_ray;
	local_ray.origin = box_matrix_inv * ray.origin;
	local_ray.direction = Vec3(box_matrix_inv * Vec4(ray.direction, 0.f));

	Hit_Result hit = test_ray_aligned_box(local_ray, aligned_box);
	if (!hit.has_hit)
		return Hit_Result();

	// Then, if we got a hit in local-space, we will transform it back into world-space
	hit.position = box_matrix * hit.position;
	hit.normal = Vec3(box_matrix * Vec4(hit.normal, 0.f));
	return hit;
}

Hit_Result test_ray_shape(const Ray& ray, const Convex_Shape* shape)
{
	Hit_Result best_hit;

	// Trace against each triangle
	for(u32 tri_index = 0; tri_index < shape->num_tris; ++tri_index)
	{
		Triangle& tri = shape->triangles[tri_index];

		// We can't hit a face on the back-side
		if (dot(tri.normal, ray.direction) > 0.f)
			continue;

		// First trace to the plane defining the face
		//	(we use the first vertex of the face as the plane point)
		Plane face_plane = plane_make(tri.verts[0], tri.normal);
		Hit_Result plane_hit = test_ray_plane(ray, face_plane);
		if (!plane_hit.has_hit)
			continue;

		// Do an early-out test first
		float hit_distance_sqrd = distance_sqrd(tri.centroid, plane_hit.position);
		if (hit_distance_sqrd > tri.radius_sqrd)
			continue;

		pix_event_scope("RayShapeFull");

		// Make sure the plane is within all the edges
		bool inside_face = true;
		for(u32 v=0; v<3; ++v)
		{
			// For each vertex, check that the ray hit is counter-clockwise to
			//	the edge going to the next vertex
			// If its clockwise the hit is outside of this edge
			Vec3 a = tri.verts[v];
			Vec3 b = tri.verts[(v + 1) % 3];

			// If the two vertices and the hit makes a counter-clockwise triangle,
			//	its inside the triangle as a whole
			if (!is_counter_clockwise(a, b, plane_hit.position, tri.normal))
			{
				inside_face = false;
				break;
			}
		}

		if (!inside_face)
			continue;

		if (!best_hit.has_hit || best_hit.time > plane_hit.time)
			return plane_hit;
	}

	return best_hit;
}

Hit_Result convert_ray_hit_to_line_trace_hit(const Hit_Result& ray_hit, const Line_Trace& line)
{
	if (!ray_hit.has_hit)
		return Hit_Result();

	// Check if the the ray hit happened too far away for the line trace
	float line_length_sqrd = distance_sqrd(line.to, line.from);

	if (square(ray_hit.time) > line_length_sqrd)
		return Hit_Result();

	// Close enough! Convert time from distance along ray
	//	to percentage of line trace
	Hit_Result line_hit = ray_hit;
	line_hit.time /= sqrt(line_length_sqrd);

	return line_hit;
}

Hit_Result test_line_trace_sphere(const Line_Trace& line, const Sphere& sphere)
{
	Ray line_ray = ray_from_to(line.from, line.to);
	Hit_Result ray_hit = test_ray_sphere(line_ray, sphere);

	return convert_ray_hit_to_line_trace_hit(ray_hit, line);
}

Hit_Result test_line_trace_plane(const Line_Trace& line, const Plane& plane)
{
	Ray line_ray = ray_from_to(line.from, line.to);
	Hit_Result ray_hit = test_ray_plane(line_ray, plane);

	return convert_ray_hit_to_line_trace_hit(ray_hit, line);
}

Hit_Result test_line_trace_aligned_box(const Line_Trace& line, const Aligned_Box& box)
{
	Ray line_ray = ray_from_to(line.from, line.to);
	Hit_Result ray_hit = test_ray_aligned_box(line_ray, box);

	return convert_ray_hit_to_line_trace_hit(ray_hit, line);
}

Hit_Result test_line_trace_box(const Line_Trace& line, const Box& box)
{
	Ray line_ray = ray_from_to(line.from, line.to);
	Hit_Result ray_hit = test_ray_box(line_ray, box);

	return convert_ray_hit_to_line_trace_hit(ray_hit, line);
}

Hit_Result test_line_trace_shape(const Line_Trace& line, const Convex_Shape* shape)
{
	Ray line_ray = ray_from_to(line.from, line.to);

	// First check if the line intersects the bounding box
	Hit_Result box_hit = test_line_trace_aligned_box(line, shape->bounding_box);
	if (!box_hit.has_hit)
		return Hit_Result();

	Hit_Result ray_hit = test_ray_shape(line_ray, shape);
	return convert_ray_hit_to_line_trace_hit(ray_hit, line);
}