#include "HitTest.h"

Hit_Result hit_make(const Vec3& position, const Vec3& normal, float time)
{
	Hit_Result hit;
	hit.has_hit = true;
	hit.position = position;
	hit.normal = normal;
	hit.time = time;

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
		// Ray origin is inside the sphere, we have a start-penetrating scenario
		return hit_make(ray.origin, normalize(-ray_to_sphere), 0.f);
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

Hit_Result test_line_sphere(const Line& line, const Sphere& sphere)
{
	float line_length_sqrd = length_sqrd(line.end - line.start);

	// Do a ray-sphere intersection test, and see if the intersection happened 
	// close enough to be within the line bounds
	Ray line_ray = ray_from_to(line.start, line.end);

	Hit_Result ray_hit = test_ray_sphere(line_ray, sphere);

	// Intersection happened outside of line bounds, disregard
	if (square(ray_hit.time) > line_length_sqrd)
		return Hit_Result();

	return ray_hit;
}

Hit_Result test_line_plane(const Line& line, const Plane& plane)
{
	float line_length_sqrd = length_sqrd(line.end - line.start);

	// Do a ray-sphere intersection test, and see if the intersection happened 
	// close enough to be within the line bounds
	Ray line_ray = ray_from_to(line.start, line.end);

	Hit_Result ray_hit = test_ray_plane(line_ray, plane);

	// Intersection happened outside of line bounds, disregard
	if (square(ray_hit.time) > line_length_sqrd)
		return Hit_Result();

	return ray_hit;
}