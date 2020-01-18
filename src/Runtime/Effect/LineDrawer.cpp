#include "LineDrawer.h"
#include "Core/Math/Math.h"
#include "Engine/Graphics/Material.h"
#include "Runtime/Game/Scene.h"

#if CLIENT

void line_drawer_init(Line_Drawer* drawer, const Vec3& origin)
{
	drawer->material = material_load("Material/line.mat");

	drawer->num_segments = 0;
	drawer->position = origin;
	drawer->timer = 0.5f;
	drawer->lifetime = 0.f;
	drawer->segments[0].position = origin;

	// Create the actual mesh
	mesh_create(&drawer->mesh);
	mesh_add_buffers(&drawer->mesh, 1);
	mesh_add_buffer_mapping(&drawer->mesh, 0, 0, 3); // Position
	mesh_add_buffer_mapping(&drawer->mesh, 0, 1, 1); // Time
	mesh_add_buffer_mapping(&drawer->mesh, 0, 2, 1); // Width

	Vec3 vec_pos = Vec3(0.f, 0.f, 1.f);
	mesh_buffer_data(&drawer->mesh, 0, &vec_pos, sizeof(vec_pos), Mesh_Storage_Stream);
	drawer->mesh.draw_mode = GL_LINE_STRIP;
	drawer->mesh.use_elements = false;

	drawer->segments[0].time = 0.f;
	drawer->segments[0].width = 0.f;
}

void line_drawer_free(Line_Drawer* drawer)
{
	mesh_free(&drawer->mesh);
}

void line_drawer_add_segment(Line_Drawer* drawer)
{
	if (drawer->num_segments < 19)
	{
		drawer->num_segments++;
	}
	else
	{
		// Move everything back
		memmove(drawer->segments, drawer->segments + 1, sizeof(Line_Segment) * 19);
	}

	drawer->segments[drawer->num_segments].time = 0.f;

}

void line_drawer_update(Line_Drawer* drawer)
{
	drawer->lifetime += time_delta();

	for(u32 i=0; i<=drawer->num_segments; ++i)
	{
		drawer->segments[i].time += time_delta();
	}

	drawer->timer += time_delta();
	if (drawer->should_destroy)
	{
		if (drawer->timer > 5.f)
		{
			scene_destroy_line_drawer(drawer);
		}
	}
	else
	{
		if (drawer->timer > 0.5f)
		{
			line_drawer_add_segment(drawer);
			drawer->timer = 0.f;
		}
	}

	drawer->segments[drawer->num_segments].position = drawer->position;
	drawer->segments[drawer->num_segments].width = min(0.2f, drawer->lifetime);

	mesh_buffer_data(&drawer->mesh, 0, drawer->segments, sizeof(Line_Segment) * (drawer->num_segments + 1));
	drawer->mesh.draw_count = drawer->num_segments + 1;
}

void line_drawer_render(Line_Drawer* drawer, const Render_State& state)
{
	material_bind(drawer->material);
	material_set(drawer->material, "u_ViewProjection", state.view_projection);

	line_drawer_update(drawer);
	if (drawer->num_segments == 0)
		return;

	mesh_draw(&drawer->mesh);
}

#endif