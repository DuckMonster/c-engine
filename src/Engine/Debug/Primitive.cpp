#include "Primitive.h"
#include "Engine/Graphics/Material.h"

const u32 sphere_ring_resolution = 32;
const u32 sphere_num_rings = 5;

u32 build_sphere_rings(Vec3* verts, const Vec3& axis_a, const Vec3& axis_b)
{
	float angle_delta = TAU / (sphere_ring_resolution);
	float ring_height_delta = 2.f / (sphere_num_rings + 1);

	const Vec3 plane_normal = cross(axis_a, axis_b);
	u32 offset = 0;

	for(u32 s=0; s<sphere_num_rings; ++s)
	{
		// "1 -" because we want the distance from sphere center
		// However, we're iterating from the top to bottom, distance wise
		float ring_height = 1.f - ring_height_delta * (s + 1);

		// a^2 + b^2 = 1
		// a^2 = 1 - b^2
		// a = sqrt(1 - b^2)
		float ring_scale = sqrt(1.f - square(ring_height));

		for(u32 i=0; i<sphere_ring_resolution; ++i)
		{
			float angle = angle_delta * i;
			float angle_next = angle_delta * (i + 1);
			verts[offset++] = (axis_a * cos(angle) + axis_b * sin(angle)) * ring_scale + plane_normal * ring_height;
			verts[offset++] = (axis_a * cos(angle_next) + axis_b * sin(angle_next)) * ring_scale + plane_normal * ring_height;
		}
	}

	return offset;
}

Vec3* make_sphere_primitive_mesh(u32* num_verts)
{
	float angle_delta = TAU / (sphere_ring_resolution);

	// +1 since we want to include the first vertex twice, to complete the loop
	// Also, each vertex is added twice, since its the end of one line, and the start of the next line
	// Then, a final *3 since its three rings, one for each cardinal plane
	*num_verts = ((sphere_ring_resolution + 1) * 2) * sphere_num_rings * 3;

	Vec3* verts = new Vec3[*num_verts];
	u32 offset = 0;

	// X-Y ring
	offset += build_sphere_rings(verts, Vec3_X, Vec3_Y);
	offset += build_sphere_rings(verts + offset, Vec3_X, Vec3_Z);
	offset += build_sphere_rings(verts + offset, Vec3_Y, Vec3_Z);

	return verts;
}

u32 build_box_lines(Vec3* verts, const Vec3& axis_a, const Vec3& axis_b)
{
	Vec3 normal_axis = cross(axis_a, axis_b);

	u32 num_verts = 0;
	verts[num_verts++] = (axis_a + axis_b + normal_axis) * 0.5f;
	verts[num_verts++] = (axis_a + axis_b - normal_axis) * 0.5f;

	verts[num_verts++] = (axis_a - axis_b + normal_axis) * 0.5f;
	verts[num_verts++] = (axis_a - axis_b - normal_axis) * 0.5f;

	verts[num_verts++] = (-axis_a - axis_b + normal_axis) * 0.5f;
	verts[num_verts++] = (-axis_a - axis_b - normal_axis) * 0.5f;

	verts[num_verts++] = (-axis_a + axis_b + normal_axis) * 0.5f;
	verts[num_verts++] = (-axis_a + axis_b - normal_axis) * 0.5f;

	return num_verts;
}

Vec3* make_box_primitive_mesh(u32* num_verts)
{
	Vec3* verts = new Vec3[8 * 3];
	u32 offset = 0;
	offset += build_box_lines(verts + offset, Vec3_X, Vec3_Y);
	offset += build_box_lines(verts + offset, Vec3_X, Vec3_Z);
	offset += build_box_lines(verts + offset, Vec3_Y, Vec3_Z);

	*num_verts = offset;
	return verts;
}

void primitive_add(Primitive_Manager* manager, Primitive_Type type, void* ptr, float duration)
{
	Primitive_Draw* draw = arena_malloc_t(&manager->arena, Primitive_Draw, 1);
	draw->type = type;
	draw->ptr = ptr;
	draw->remove_time = time_elapsed() + duration;
	draw->next = draw->prev = nullptr;

	// Push to the front of the list
	if (manager->draw_list)
	{
		manager->draw_list->prev = draw;
		draw->next = manager->draw_list;
	}

	manager->draw_list = draw;
}

void primitives_init(Primitive_Manager* manager)
{
	arena_init(&manager->arena);

	// Line stuff
	manager->line_material = material_load("Material/Primitive/prim_line.mat");

	Vec2 line_verts[] = { Vec2(0.f, 0.f), Vec2(1.f, 0.f) };
	mesh_create(&manager->line_mesh);
	mesh_add_buffers(&manager->line_mesh, 1);
	mesh_add_buffer_mapping(&manager->line_mesh, 0, 0, 2);
	mesh_buffer_data(&manager->line_mesh, 0, line_verts, sizeof(line_verts));
	manager->line_mesh.draw_mode = GL_LINES;

	// Point stuff
	manager->point_material = material_load("Material/Primitive/prim_point.mat");

	Vec2 point_verts[] = { Vec2(0.f, 0.f) };
	mesh_create(&manager->point_mesh);
	mesh_add_buffers(&manager->point_mesh, 1);
	mesh_add_buffer_mapping(&manager->point_mesh, 0, 0, 2);
	mesh_buffer_data(&manager->point_mesh, 0, point_verts, sizeof(point_verts));
	manager->point_mesh.draw_mode = GL_POINTS;

	manager->mesh_material = material_load("Material/Primitive/prim_mesh.mat");

	// Sphere stuff
	u32 sphere_num_verts;
	Vec3* sphere_verts = make_sphere_primitive_mesh(&sphere_num_verts);
	mesh_create(&manager->sphere_mesh);
	mesh_add_buffers(&manager->sphere_mesh, 1);
	mesh_add_buffer_mapping(&manager->sphere_mesh, 0, 0, 3);
	mesh_buffer_data(&manager->sphere_mesh, 0, sphere_verts, sizeof(Vec3) * sphere_num_verts);
	manager->sphere_mesh.draw_mode = GL_LINES;

	delete sphere_verts;

	// Box stuff
	u32 box_num_verts;
	Vec3* box_verts = make_box_primitive_mesh(&box_num_verts);
	mesh_create(&manager->box_mesh);
	mesh_add_buffers(&manager->box_mesh, 1);
	mesh_add_buffer_mapping(&manager->box_mesh, 0, 0, 3);
	mesh_buffer_data(&manager->box_mesh, 0, box_verts, sizeof(Vec3) * box_num_verts);
	manager->box_mesh.draw_mode = GL_LINES;

	delete box_verts;
}

void primitives_render(Primitive_Manager* manager, const Render_State& state)
{
	float time = time_elapsed();
	Primitive_Draw* draw = manager->draw_list;

	while(draw)
	{
		switch(draw->type)
		{
			case PRIM_Line:
			{
				Primitive_Line* line = (Primitive_Line*)draw->ptr;
				Vec3 origin = line->from;
				Vec3 delta = line->to - line->from;

				Mat4 line_matrix;
				line_matrix[0] = Vec4(delta, 0.f);
				line_matrix[3] = Vec4(origin, 1.f);

				glLineWidth(1.f);

				material_bind(manager->line_material);
				material_set(manager->line_material, "u_LineMatrix", line_matrix);
				material_set(manager->line_material, "u_ViewProjection", state.view_projection);
				material_set(manager->line_material, "u_Color", line->color);

				mesh_draw(&manager->line_mesh);
				break;
			}

			case PRIM_Point:
			{
				Primitive_Point* point = (Primitive_Point*)draw->ptr;
				Mat4 point_matrix;
				point_matrix[3] = Vec4(point->position, 1.f);

				glPointSize(4.f);

				material_bind(manager->point_material);
				material_set(manager->point_material, "u_PointMatrix", point_matrix);
				material_set(manager->point_material, "u_ViewProjection", state.view_projection);
				material_set(manager->point_material, "u_Color", point->color);

				mesh_draw(&manager->point_mesh);
				break;
			}

			case PRIM_Sphere:
			{
				Primitive_Sphere* sphere = (Primitive_Sphere*)draw->ptr;
				Mat4 model = mat_position_scale(sphere->origin, Vec3(sphere->radius));

				material_bind(manager->mesh_material);
				material_set(manager->mesh_material, "u_Model", model);
				material_set(manager->mesh_material, "u_ViewProjection", state.view_projection);
				material_set(manager->mesh_material, "u_Color", sphere->color);

				mesh_draw(&manager->sphere_mesh);
				break;
			}

			case PRIM_Box:
			{
				Primitive_Box* box= (Primitive_Box*)draw->ptr;
				Mat4 model = mat_position_rotation_scale(box->position, box->orientation, box->size);

				material_bind(manager->mesh_material);
				material_set(manager->mesh_material, "u_Model", model);
				material_set(manager->mesh_material, "u_ViewProjection", state.view_projection);
				material_set(manager->mesh_material, "u_Color", box->color);

				mesh_draw(&manager->box_mesh);
				break;
			}
		}

		Primitive_Draw* next = draw->next;

		// Check if this primitive should be popped from the list
		//if (time > draw->remove_time)
			//primitive_remove(manager, draw);

		draw = next;
	}

	arena_clear(&manager->arena);
	manager->draw_list = nullptr;
}

void primitive_draw_line(Primitive_Manager* manager, const Vec3& from, const Vec3& to, const Vec4& color, float duration)
{
	Primitive_Line* line = arena_malloc_t(&manager->arena, Primitive_Line, 1);
	line->from = from;
	line->to = to;
	line->color = color;

	primitive_add(manager, PRIM_Line, line, duration);
}

void primitive_draw_point(Primitive_Manager* manager, const Vec3& position, const Vec4& color, float duration)
{
	Primitive_Point* point = arena_malloc_t(&manager->arena, Primitive_Point, 1);
	point->position = position;
	point->color = color;

	primitive_add(manager, PRIM_Point, point, duration);
}

void primitive_draw_sphere(Primitive_Manager* manager, const Vec3& origin, float radius, const Vec4& color, float duration)
{
	Primitive_Sphere* sphere = arena_malloc_t(&manager->arena, Primitive_Sphere, 1);
	sphere->origin = origin;
	sphere->radius = radius;
	sphere->color = color;

	primitive_add(manager, PRIM_Sphere, sphere, duration);
}

void primitive_draw_box(Primitive_Manager* manager, const Vec3& position, const Vec3& size, const Quat& orientation, const Vec4& color, float duration)
{
	Primitive_Box* box = arena_malloc_t(&manager->arena, Primitive_Box, 1);
	box->position = position;
	box->size = size;
	box->orientation = orientation;
	box->color = color;

	primitive_add(manager, PRIM_Box, box, duration);
}