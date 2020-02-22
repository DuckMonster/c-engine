#pragma once
#include "Engine/Graphics/Mesh.h"

#define MAX_SPIKES 40

struct Material;
struct Render_State;

struct Fx_Spike_Params
{
	Vec3 from;
	Vec3 to;
	float size = 1.f;
	float center_alpha = 0.5f;
	float duration = 1.f;

	// Movement
	// Delta is length to move in the (from - to) direction through the duration
	float from_delta = 0.f;
	float to_delta = 0.f;
	float translate_delta = 0.f;
	float center_delta = 0.f;

	float move_exponent = 4.f;
	float size_exponent = 2.f;
};

struct Fx_Spike
{
	bool enabled = false;
	Fx_Spike_Params params;

	float timer = 0.f;

	Mesh mesh;
};

struct Fx_Spikes
{
	Fx_Spike* spikes = nullptr;
	const Material* material = nullptr;
};

#if CLIENT
void fx_spike_init(Fx_Spike* spike, const Fx_Spike_Params& params);
void fx_spike_free(Fx_Spike* spike);
void fx_spike_update(Fx_Spike* spike);

void fx_spikes_init(Fx_Spikes* spikes);
void fx_spikes_render(Fx_Spikes* spikes, const Render_State& state);

void fx_spike_make(Fx_Spikes* spikes, const Fx_Spike_Params& params);
#endif