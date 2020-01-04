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
	Vec3 to_plane = plane.point - ray.origin;
	float dist = dot(to_plane, plane.normal);
	float grad = dot(ray.direction, plane.normal);

	// If grad and dist are not the same sign, the intersection point is behind the ray origin
	if (grad * dist < 0)
		return Hit_Result();

	return hit_make(ray.origin + ray.direction * (dist / grad), plane.normal);
}

Hit_Result test_ray_sphere(const Ray& ray, const Sphere& sphere)
{
	float radius_sqrd = square(sphere.radius);

	// Check if the ray is inside the sphere, in which we just early out
	Vec3 sphere_to_ray = ray.origin - sphere.origin;
	if (length_sqrd(sphere_to_ray) <= radius_sqrd)
		return hit_make(ray.origin, normalize(sphere_to_ray));

	Vec3 closest_point = constrain_to_direction(sphere.origin - ray.origin, ray.direction);

	// Closest point on ray is further away than radius, no intersection
	if (distance_sqrd(closest_point, sphere.origin) > radius_sqrd)
		return Hit_Result();

	return Hit_Result();
}

Hit_Result test_line_sphere(const Line& line, const Sphere& sphere)
{
	return Hit_Result();
}