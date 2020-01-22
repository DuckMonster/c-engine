#pragma once
#include "FxParticle.h"
#include "Runtime/Render/Render.h"

struct Fx_Manager
{
	Fx_Particles* particles;
};

#if CLIENT
extern Fx_Manager fx_manager;

void fx_init();
void fx_update();
void fx_render(const Render_State& state);
void fx_make_particle(const Fx_Particle_Spawn_Params& params);

#endif