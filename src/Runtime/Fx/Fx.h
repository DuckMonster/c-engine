#pragma once
#include "FxParticle.h"
#include "FxSpike.h"
#include "Runtime/Render/Render.h"

struct Fx_Resource;

struct Fx_Manager
{
	Fx_Particles* particles;
	Fx_Spikes* spikes;
};

#if CLIENT
extern Fx_Manager fx_manager;

void fx_init();
void fx_update();
void fx_render(const Render_State& state);
void fx_make_particle(const Fx_Particle_Spawn_Params& params);
void fx_make_spike(const Fx_Spike_Params& params);
void fx_make_res(const Fx_Resource* resource, const Fx_Params& params);

#endif