#include "HitTest.h"

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
		// Ray origin is inside the sphere, we have a start-penetrating
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
		return Hit_Result();

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

Hit_Result test_line_trace_sphere(const Line_Trace& line, const Sphere& sphere)
{
	float line_length = length(line.end - line.start);

	// Do a ray-sphere intersection test, and see if the intersection happened 
	// close enough to be within the line bounds
	Ray line_ray = ray_from_to(line.start, line.end);

	Hit_Result ray_hit = test_ray_sphere(line_ray, sphere);

	// Intersection happened outside of line bounds, disregard
	if (ray_hit.time > line_length)
		return Hit_Result();

	// Convert from absolute distance to percentage
	ray_hit.time /= line_length;
	return ray_hit;
}

Hit_Result test_line_trace_plane(const Line_Trace& line, const Plane& plane)
{
	float line_length = length(line.end - line.start);

	// Do a ray-sphere intersection test, and see if the intersection happened 
	// close enough to be within the line bounds
	Ray line_ray = ray_from_to(line.start, line.end);

	Hit_Result ray_hit = test_ray_plane(line_ray, plane);

	// Intersection happened outside of line bounds, disregard
	if (ray_hit.time > line_length)
		return Hit_Result();

	// Convert from absolute distance to percentage
	ray_hit.time /= line_length;
	return ray_hit;
}

Hit_Result test_line_trace_aligned_box(const Line_Trace& line, const Aligned_Box& box)
{
	float line_length = length(line.end - line.start);

	// Do a ray-sphere intersection test, and see if the intersection happened 
	// close enough to be within the line bounds
	Ray line_ray = ray_from_to(line.start, line.end);

	Hit_Result ray_hit = test_ray_aligned_box(line_ray, box);

	// Intersection happened outside of line bounds, disregard
	if (ray_hit.time > line_length)
		return Hit_Result();

	// Convert from absolute distance to percentage
	ray_hit.time /= line_length;
	return ray_hit;
}

Hit_Result test_line_trace_box(const Line_Trace& line, const Box& box)
{
	float line_length = length(line.end - line.start);

	// Do a ray-sphere intersection test, and see if the intersection happened 
	// close enough to be within the line bounds
	Ray line_ray = ray_from_to(line.start, line.end);

	Hit_Result ray_hit = test_ray_box(line_ray, box);

	// Intersection happened outside of line bounds, disregard
	if (ray_hit.time > line_length)
		return Hit_Result();

	// Convert from absolute distance to percentage
	ray_hit.time /= line_length;
	return ray_hit;
}