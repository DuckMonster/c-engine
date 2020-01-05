#include "Primitive.h"
#include "Engine/Graphics/Material.h"

Vec3* make_sphere_primitive_mesh(u32* num_verts)
{
	const u32 ring_resolution = 32;
	float angle_delta = TAU / (ring_resolution);

	// +1 since we want to include the first vertex twice, to complete the loop
	// Also, each vertex is added twice, since its the end of one line, and the start of the next line
	// Then, a final *3 since its three rings, one for each cardinal plane
	*num_verts = ((ring_resolution + 1) * 2) * 3;

	Vec3* verts = new Vec3[*num_verts];
	u32 offset = 0;

	// X-Y ring
	for(u32 i=0; i<ring_resolution; ++i)
	{
		float angle = angle_delta * i;
		float angle_next = angle_delta * (i + 1);
		verts[offset++] = Vec3(cos(angle), sin(angle), 0.f);
		verts[offset++] = Vec3(cos(angle_next), sin(angle_next), 0.f);
	}

	// X-Z ring
	for(u32 i=0; i<ring_resolution; ++i)
	{
		float angle = angle_delta * i;
		float angle_next = angle_delta * (i + 1);
		verts[offset++] = Vec3(cos(angle), 0.f, sin(angle));
		verts[offset++] = Vec3(cos(angle_next), 0.f, sin(angle_next));
	}

	// Y-Z ring
	for(u32 i=0; i<ring_resolution; ++i)
	{
		float angle = angle_delta * i;
		float angle_next = angle_delta * (i + 1);
		verts[offset++] = Vec3(0.f, cos(angle), sin(angle));
		verts[offset++] = Vec3(0.f, cos(angle_next), sin(angle_next));
	}

	return verts;
}

void primitive_add(Primitive_Manager* manager, Primitive_Type type, void* ptr, float duration)
{
	Primitive_Draw* draw = new Primitive_Draw();
	draw->type = type;
	draw->ptr = ptr;
	draw->remove_time = time_duration() + duration;
	draw->next = draw->prev = nullptr;

	// Push to the front of the list
	if (manager->draw_list)
	{
		manager->draw_list->prev = draw;
		draw->next = manager->draw_list;
	}

	manager->draw_list = draw;
}

void primitive_remove(Primitive_Manager* manager, Primitive_Draw* draw)
{
	// Re-link list
	if (draw->prev)
		draw->prev->next = draw->next;
	if (draw->next)
		draw->next->prev = draw->prev;

	// If this is the first element, re-set it
	if (manager->draw_list == draw)
		manager->draw_list = draw->next;

	delete draw->ptr;
	delete draw;
}

void primitives_init(Primitive_Manager* manager)
{
	// Line stuff
	manager->line_material = material_load("Material/Primitive/prim_line.mat");

	Vec2 line_verts[] = { Vec2(0.f, 0.f), Vec2(1.f, 0.f) };
	mesh_create(&manager->line_mesh);
	mesh_add_buffers(&manager->line_mesh, 1);
	mesh_map_buffer(&manager->line_mesh, 0, 0, 2, 2, 0);
	mesh_buffer_data(&manager->line_mesh, 0, line_verts, sizeof(line_verts));
	manager->line_mesh.draw_mode = GL_LINES;
	manager->line_mesh.draw_count = 2;

	// Point stuff
	manager->point_material = material_load("Material/Primitive/prim_point.mat");

	Vec2 point_verts[] = { Vec2(0.f, 0.f) };
	mesh_create(&manager->point_mesh);
	mesh_add_buffers(&manager->point_mesh, 1);
	mesh_map_buffer(&manager->point_mesh, 0, 0, 2, 2, 0);
	mesh_buffer_data(&manager->point_mesh, 0, point_verts, sizeof(point_verts));
	manager->point_mesh.draw_mode = GL_POINTS;
	manager->point_mesh.draw_count = 1;

	// Sphere stuff
	manager->sphere_material = material_load("Material/Primitive/prim_sphere.mat");

	u32 sphere_num_verts;
	Vec3* sphere_verts = make_sphere_primitive_mesh(&sphere_num_verts);
	mesh_create(&manager->sphere_mesh);
	mesh_add_buffers(&manager->sphere_mesh, 1);
	mesh_map_buffer(&manager->sphere_mesh, 0, 0, 3, 3, 0);
	mesh_buffer_data(&manager->sphere_mesh, 0, sphere_verts, sizeof(Vec3) * sphere_num_verts);
	manager->sphere_mesh.draw_mode = GL_LINES;
	manager->sphere_mesh.draw_count = sphere_num_verts;
}

void primitives_render(Primitive_Manager* manager, const Render_State& state)
{
	float time = time_duration();
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
				Mat4 sphere_matrix = mat_position_scale(sphere->origin, Vec3(sphere->radius));

				material_bind(manager->sphere_material);
				material_set(manager->sphere_material, "u_SphereMatrix", sphere_matrix);
				material_set(manager->sphere_material, "u_ViewProjection", state.view_projection);
				material_set(manager->sphere_material, "u_Color", sphere->color);

				mesh_draw(&manager->sphere_mesh);
				break;
			}
		}

		Primitive_Draw* next = draw->next;

		// Check if this primitive should be popped from the list
		if (time > draw->remove_time)
			primitive_remove(manager, draw);

		draw = next;
	}
}

void primitive_draw_line(Primitive_Manager* manager, const Vec3& from, const Vec3& to, const Vec4& color, float duration)
{
	Primitive_Line* line = new Primitive_Line();
	line->from = from;
	line->to = to;
	line->color = color;

	primitive_add(manager, PRIM_Line, line, duration);
}

void primitive_draw_point(Primitive_Manager* manager, const Vec3& position, const Vec4& color, float duration)
{
	Primitive_Point* point = new Primitive_Point();
	point->position = position;
	point->color = color;

	primitive_add(manager, PRIM_Point, point, duration);
}

void primitive_draw_sphere(Primitive_Manager* manager, const Vec3& origin, float radius, const Vec4& color, float duration)
{
	Primitive_Sphere* sphere = new Primitive_Sphere();
	sphere->origin = origin;
	sphere->radius = radius;
	sphere->color = color;

	primitive_add(manager, PRIM_Sphere, sphere, duration);
}