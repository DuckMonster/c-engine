#include "FxParticle.h"
#include "Engine/Graphics/Material.h"
#include "Runtime/Game/Scene.h"
#include "Runtime/Game/SceneQuery.h"

#if CLIENT

void fx_particles_init(Fx_Particles* particles)
{
	// Create mesh
	mesh_create(&particles->mesh);
	mesh_add_buffers(&particles->mesh, 1);
	mesh_add_buffer_mapping(&particles->mesh, 0, 0, 3);
	mesh_add_buffer_mapping(&particles->mesh, 0, 1, 4);
	particles->mesh.draw_mode = GL_POINTS;

	// Create material
	particles->material = material_load("Material/Fx/Particle.mat");

	// Create lists
	particles->particles = new Fx_Particle[PARTICLES_MAX];
	particles->data = new Fx_Particle_Data[PARTICLES_MAX];

	mem_zero(particles->particles, sizeof(Fx_Particle) * PARTICLES_MAX);
	mem_zero(particles->data, sizeof(Fx_Particle_Data) * PARTICLES_MAX);

	mesh_buffer_data(&particles->mesh, 0, particles->particles, sizeof(Fx_Particle) * PARTICLES_MAX, Mesh_Storage_Stream);
}

void fx_particles_update(Fx_Particles* particles)
{
	float current_time = time_elapsed();

	for(u32 i=0; i<PARTICLES_MAX; ++i)
	{
		Fx_Particle_Data& data = particles->data[i];
		Fx_Particle& particle = particles->particles[i];

		if (!data.enabled)
		{
			particle.position = Vec3_NaN;
			continue;
		}

		if (data.expire_time < current_time)
		{
			data.enabled = false;
			continue;
		}

		// Apply physics to the velocity
		data.velocity += -Vec3_Z * data.gravity * time_delta();
		data.velocity -= data.velocity * data.drag * time_delta();

		// Line trace to see if we hit any surfaces!
		Vec3 pos = particles->particles[i].position;
		Vec3 move_delta = particles->data[i].velocity * time_delta();

		Line_Trace move_line;
		move_line.from = pos;
		move_line.to = pos + move_delta;

		Scene_Query_Params params;
		params.mask = QUERY_Props;

		//Scene_Query_Result result = scene_query_line(move_line, params);
		Scene_Query_Result result;
		if (result.hit.has_hit)
		{
			// If we did, bounce against it!
			data.velocity = reflect(data.velocity, result.hit.normal) * data.bounce;
		}

		particle.position += data.velocity * time_delta();
	}

	mesh_buffer_data(&particles->mesh, 0, particles->particles, sizeof(Fx_Particle) * PARTICLES_MAX, Mesh_Storage_Stream);
}

void fx_particles_render(Fx_Particles* particles, const Render_State& state)
{
	glPointSize(1.f);
	material_bind(particles->material);
	material_set(particles->material, "u_ViewProjection", state.view_projection);
	mesh_draw(&particles->mesh);
}

void fx_particle_make(Fx_Particles* particles, const Fx_Particle_Spawn_Params& params)
{
	float current_time = time_elapsed();

	u32 num = params.num_particles;
	for(u32 i=0; i < PARTICLES_MAX && num > 0; ++i)
	{
		if (particles->data[i].enabled)
			continue;

		Vec3 position = params.position + random_point_on_sphere() * params.position_radius;
		float lifetime = lerp(params.lifetime_min, params.lifetime_max, random_float());

		Quat cone_quat =
			quat_from_x(params.velocity) *
			angle_axis(random_float(TAU), Vec3_X) *
			angle_axis(random_float(radians(params.velocity_cone_angle)), Vec3_Y);

		Vec3 velocity = quat_x(cone_quat) * length(params.velocity);
		velocity *= 1.f + params.velocity_scale_variance * random_float(-0.5f, 0.5f);

		particles->data[i].enabled = true;
		particles->data[i].velocity = velocity;
		particles->data[i].expire_time = current_time + lifetime;
		particles->data[i].gravity = lerp(params.gravity_min, params.gravity_max, random_float());
		particles->data[i].drag = lerp(params.drag_min, params.drag_max, random_float());
		particles->data[i].bounce = lerp(params.bounce_min, params.bounce_max, random_float());
		particles->particles[i].position = position;
		particles->particles[i].color = lerp(params.color_min, params.color_max, random_float());
		num--;
	}
}
#endif