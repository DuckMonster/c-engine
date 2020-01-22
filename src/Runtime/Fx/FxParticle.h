#pragma once
#include "Engine/Graphics/Mesh.h"
#include "Runtime/Render/Render.h"
#define PARTICLES_MAX 256

struct Material;

struct Fx_Particle_Spawn_Params
{
	u32 num_particles = 0;

	// Location
	Vec3 position;
	float position_radius = 0.f;

	// Lifetime
	float lifetime_min = 1.f;
	float lifetime_max = 2.f;

	// Physics
	Vec3 velocity;
	float velocity_cone_angle = 0.f;
	float velocity_scale_variance = 0.f;

	float gravity_min = 7.f;
	float gravity_max = 10.f;
	float drag_min = 1.2f;
	float drag_max = 2.1f;
	float bounce_min = 0.5f;
	float bounce_max = 0.9f;

	// Visuals
	Vec4 color_min = Color_White;
	Vec4 color_max = Color_White;
};

struct Fx_Particle_Data
{
	bool enabled;
	Vec3 velocity;
	float expire_time;
	float gravity;
	float drag;
	float bounce;
};

struct Fx_Particle
{
	Vec3 position;
	Vec4 color;
};

struct Fx_Particles
{
	Fx_Particle* particles;
	Fx_Particle_Data* data;
	Mesh mesh;
	const Material* material;
};

#if CLIENT
void fx_particles_init(Fx_Particles* particles);
void fx_particles_update(Fx_Particles* particles);
void fx_particles_render(Fx_Particles* particles, const Render_State& state);

void fx_particle_make(Fx_Particles* particles, const Fx_Particle_Spawn_Params& params);
#endif