#include "Fx.h"

#if CLIENT
Fx_Manager fx_manager;

void fx_init()
{
	fx_manager.particles = new Fx_Particles();
	fx_particles_init(fx_manager.particles);

	fx_manager.spikes = new Fx_Spikes();
	fx_spikes_init(fx_manager.spikes);
}

void fx_update()
{
	fx_particles_update(fx_manager.particles);
}

void fx_render(const Render_State& state)
{
	fx_particles_render(fx_manager.particles, state);
	fx_spikes_render(fx_manager.spikes, state);
}

void fx_make_particle(const Fx_Particle_Spawn_Params& params)
{
	fx_particle_make(fx_manager.particles, params);
}

void fx_make_spike(const Fx_Spike_Params& params)
{
	fx_spike_make(fx_manager.spikes, params);
}

#endif