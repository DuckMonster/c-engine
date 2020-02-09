#include "Grass.h"
#include "Engine/Graphics/Material.h"
#include "Runtime/Render/ColorMap.h"

#if CLIENT

struct Grass_Segment
{
	float height;
	float half_width;
};

struct Grass_Vert
{
	Vec2 local_position;
	Vec3 origin_position;
};

void grass_init(Grass* grass)
{
	const u32 num_segments = 4;

	Grass_Segment segments[num_segments];
	segments[0].height = 0.f;
	segments[0].half_width = 0.05f;
	segments[1].height = 0.18f;
	segments[1].half_width = 0.03f;
	segments[2].height = 0.32f;
	segments[2].half_width = 0.02f;
	segments[3].height = 0.4f;
	segments[3].half_width = 0.f;

	mesh_create(&grass->mesh);
	mesh_add_buffers(&grass->mesh, 1);
	mesh_add_buffer_mapping(&grass->mesh, 0, 0, 2);
	mesh_add_buffer_mapping(&grass->mesh, 0, 1, 3);

	float grass_square_size = 50.f;
	u32 grass_dimensions = 280;
	u32 grass_num = grass_dimensions * grass_dimensions;
	u32 verts_per_grass = num_segments * 6;
	Grass_Vert* all_verts = new Grass_Vert[grass_num * verts_per_grass];

	for(u32 i=0; i<grass_num; ++i)
	{
		Grass_Vert* verts = all_verts + verts_per_grass * i;
		float x = (i % grass_dimensions) * (grass_square_size / grass_dimensions);
		float y = (i / grass_dimensions) * (grass_square_size / grass_dimensions);

		x -= grass_square_size / 2.f;
		y -= grass_square_size / 2.f;
		x += random_float(-0.1f, 0.1f);
		y += random_float(-0.1f, 0.1f);

		for(u32 seg_index=0; seg_index<num_segments - 1; ++seg_index)
		{
			Grass_Segment& seg = segments[seg_index];
			Grass_Segment& seg_next = segments[seg_index + 1];
			Grass_Vert* seg_verts = verts + seg_index * 6;

			seg_verts[0].local_position = Vec2(-seg.half_width, seg.height);
			seg_verts[1].local_position = Vec2(seg.half_width, seg.height);
			seg_verts[2].local_position = Vec2(-seg_next.half_width, seg_next.height);

			seg_verts[3].local_position = Vec2(seg.half_width, seg.height);
			seg_verts[4].local_position = Vec2(-seg_next.half_width, seg_next.height);
			seg_verts[5].local_position = Vec2(seg_next.half_width, seg_next.height);
		}

		for(u32 vert_index = 0; vert_index < verts_per_grass; ++vert_index)
			verts[vert_index].origin_position = Vec3(x, y, 0.f);
	}

	mesh_buffer_data(&grass->mesh, 0, all_verts, sizeof(Grass_Vert) * verts_per_grass * grass_num);

	grass->material = material_load("Material/Environment/grass.mat");
}

void grass_update(Grass* grass)
{
}

void grass_render(Grass* grass, const Render_State& state)
{
	Mat4 cam_inverse = state.view;
	cam_inverse[3] = Vec4(0.f, 0.f, 0.f, 1.f);
	cam_inverse = inverse(cam_inverse);

	material_bind(grass->material);
	material_set(grass->material, "u_ViewProjection", state.view_projection);
	material_set(grass->material, "u_InvCamera", cam_inverse);
	material_set(grass->material, "u_ColorMapSampler", COLOR_MAP_UNIFORM);
	material_set(grass->material, "u_LightViewProjection", state.light_view_projection);
	material_set(grass->material, "u_LightDirection", state.light_direction);
	material_set(grass->material, "u_Time", time_current());
	material_set(grass->material, RENDER_SHADOW_BUFFER_UNIFORM, RENDER_SHADOW_BUFFER_TEXTURE_INDEX);
	color_map_bind();

	mesh_draw(&grass->mesh);
}

#endif