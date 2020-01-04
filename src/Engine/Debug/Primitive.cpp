#include "Primitive.h"
#include "Engine/Graphics/Material.h"

void primitives_init(Primitive_Manager* manager)
{
	manager->line_material = material_load("Material/Primitive/prim_line.mat");
	mesh_create(&manager->line_mesh);
	mesh_add_buffers(&manager->line_mesh, 1);
	mesh_map_buffer(&manager->line_mesh, 0, 0, 2, 2, 0);

	Vec2 line_verts[2] = { Vec2(0.f, 0.f), Vec2(1.f, 0.f) };
	mesh_buffer_data(&manager->line_mesh, 0, line_verts, sizeof(line_verts));
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

				material_bind(manager->line_material);
				material_set(manager->line_material, "u_LineMatrix", line_matrix);
				material_set(manager->line_material, "u_ViewProjection", state.view_projection);

				mesh_draw(&manager->line_mesh);
				break;
			}
		}
	}
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

void primitive_draw_line(Primitive_Manager* manager, const Vec3& from, const Vec3& to, float duration)
{
	Primitive_Line* line = new Primitive_Line();
	line->from = from;
	line->to = to;

	primitive_add(manager, PRIM_Line, line, duration);
}