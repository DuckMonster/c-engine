#include "Fx.h"

#if CLIENT
Fx_Manager fx_manager;

void fx_init()
{
	fx_manager.particles = new Fx_Particles();
	fx_particles_init(fx_manager.particles);
}

void fx_update()
{
	fx_particles_update(fx_manager.particles);
}

void fx_render(const Render_State& state)
{
	fx_particles_render(fx_manager.particles, state);
}

void fx_make_particle(const Fx_Particle_Spawn_Params& params)
{
	fx_particle_make(fx_manager.particles, params);
}

#endif