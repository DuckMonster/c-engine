#include "LineDrawer.h"
#include "Core/Container/SparseList.h"
#include "Engine/Graphics/Material.h"
#include "Core/Math/Math.h"

Sparse_List<Line_Drawer> line_list;
const Material* line_material;

void line_drawer_init()
{
	splist_create(&line_list, 100);
	line_material = material_load("Material/line.mat");
}

Line_Drawer* line_drawer_make()
{
	Line_Drawer* drawer = splist_add(&line_list);
	drawer->num_segments = 0;
	drawer->position = Vec3_Zero;
	drawer->timer = -1.f;
	drawer->lifetime = 0.f;

	mesh_create(&drawer->mesh);
	mesh_add_buffers(&drawer->mesh, 1);
	mesh_bind_buffer(&drawer->mesh, 0, 0, 3, 5, 0);
	mesh_bind_buffer(&drawer->mesh, 0, 1, 1, 5, 3);
	mesh_bind_buffer(&drawer->mesh, 0, 2, 1, 5, 4);

	Vec3 vec_pos = Vec3(0.f, 0.f, 1.f);
	mesh_buffer_data(&drawer->mesh, 0, &vec_pos, sizeof(vec_pos));
	drawer->mesh.draw_mode = GL_LINE_STRIP;
	drawer->mesh.draw_count = 1;
	drawer->mesh.use_elements = false;

	return drawer;
}

void line_drawer_destroy(Line_Drawer* drawer)
{
	mesh_free(&drawer->mesh);
	splist_remove(&line_list, drawer);
}

void line_drawer_add_segment(Line_Drawer* drawer)
{
	if (drawer->num_segments < 19)
	{
		drawer->num_segments++;
		return;
	}
	else
	{
		// Move everything back
		memmove(drawer->segments, drawer->segments + 1, sizeof(Line_Segment) * 19);
	}
}

void line_drawer_update(Line_Drawer* drawer)
{
	drawer->segments[drawer->num_segments].position = drawer->position;
	drawer->segments[drawer->num_segments].time = 0.f;
	drawer->segments[drawer->num_segments].width = min(0.2f, drawer->lifetime);

	drawer->lifetime += time_delta();

	for(u32 i=0; i<drawer->num_segments; ++i)
	{
		drawer->segments[i].time += time_delta();
	}

	drawer->timer -= time_delta();
	if (drawer->timer <= 0.f)
	{
		line_drawer_add_segment(drawer);
		drawer->timer = 0.5f;
	}

	drawer->segments[drawer->num_segments].position = drawer->position;
	drawer->segments[drawer->num_segments].time = 0.f;
	drawer->segments[drawer->num_segments].width = min(0.2f, drawer->lifetime);

	mesh_buffer_data(&drawer->mesh, 0, drawer->segments, sizeof(Line_Segment) * (drawer->num_segments + 1));
	drawer->mesh.draw_count = drawer->num_segments + 1;
}

void line_drawer_render(const Render_State& state)
{
	glUseProgram(line_material->program);
	material_set(line_material, "u_ViewProjection", state.view_projection);

	Line_Drawer* drawer;
	SPLIST_FOREACH(&line_list, drawer)
	{
		line_drawer_update(drawer);
		if (drawer->num_segments == 0)
			continue;

		mesh_draw(&drawer->mesh);
	}
}