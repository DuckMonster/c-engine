#include "FxSpike.h"
#include "Engine/Graphics/Material.h"
#include "Runtime/Render/Render.h"

#if CLIENT
void fx_spike_build_verts(Fx_Spike* spike, Vec3* verts)
{
	Fx_Spike_Params& params = spike->params;

	// Make the center
	float move_time = 1.f - spike->timer / params.duration;
	Vec3 direction = normalize(params.base.direction);

	Vec3 from = params.base.position + direction * params.from;
	Vec3 to = params.base.position + direction * params.to;
	verts[0] = from;
	verts[3] = to;

	float center_alpha = params.center_alpha;
	Vec3 center = lerp(from, to, center_alpha);

	float size_time = spike->timer / params.duration;
	float size = params.size * size_time;

	Vec3 center_tangent = Vec3(direction.y, -direction.x, direction.z);
	verts[1] = center + center_tangent * size;
	verts[2] = center - center_tangent * size;
}

void fx_spike_init(Fx_Spike* spike, const Fx_Spike_Params& params)
{
	spike->enabled = true;
	spike->params = params;
	spike->timer = params.duration;

	// Make the mesh fool
	mesh_create(&spike->mesh);
	mesh_add_buffers(&spike->mesh, 1);

	Vec3 verts[4];
	fx_spike_build_verts(spike, verts);

	mesh_add_buffer_mapping(&spike->mesh, 0, 0, 3);
	mesh_buffer_data(&spike->mesh, 0, verts, sizeof(verts));
	spike->mesh.draw_mode = GL_TRIANGLE_STRIP;
}

void fx_spike_free(Fx_Spike* spike)
{
	spike->enabled = false;
	mesh_free(&spike->mesh);
}

void fx_spike_update(Fx_Spike* spike)
{
	spike->timer -= time_delta();
	if (spike->timer <= 0.f)
	{
		fx_spike_free(spike);
		return;
	}

	Vec3 verts[4];
	fx_spike_build_verts(spike, verts);
	mesh_buffer_data(&spike->mesh, 0, verts, sizeof(verts));
}

void fx_spikes_init(Fx_Spikes* spikes)
{
	spikes->spikes = new Fx_Spike[MAX_SPIKES];
	spikes->material = material_load("Material/Fx/Spike.mat");
}

void fx_spikes_render(Fx_Spikes* spikes, const Render_State& state)
{
	material_bind(spikes->material);
	material_set(spikes->material, "u_ViewProjection", state.view_projection);

	for(u32 i=0; i<MAX_SPIKES; ++i)
	{
		if (!spikes->spikes[i].enabled)
			continue;

		fx_spike_update(&spikes->spikes[i]);
		mesh_draw(&spikes->spikes[i].mesh);
	}
}

void fx_spike_make(Fx_Spikes* spikes, const Fx_Spike_Params& params)
{
	Fx_Spike* spike = nullptr;
	for(u32 i=0; i<MAX_SPIKES; ++i)
	{
		if (!spikes->spikes[i].enabled)
		{
			spike = &spikes->spikes[i];
			break;
		}
	}

	if (!spike)
		return;

	fx_spike_init(spike, params);
}
#endif