#include "Fx.h"
#include "FxResource.h"

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

void fx_read_value_or_range(const Dat_Object* obj, const char* expr, float* value)
{
	const Dat_Node* node = dat_get_node(obj, expr);
	if (node == nullptr)
		return;

	if (node->type == Dat_Node_Type::Array)
	{
		const Dat_Array* array = (const Dat_Array*)node;
		bool is_valid = true;

		float min;
		float max;
		is_valid &= dat_array_read(array, 0, &min);
		is_valid &= dat_array_read(array, 1, &max);

		if (!is_valid)
		{
			msg_box("Invalid float range '%s' in fx file", expr);
			return;
		}

		*value = lerp(min, max, random_float());
	}
	else if (node->type == Dat_Node_Type::ValueRaw)
	{
		const Dat_Value_Raw* value_node = (const Dat_Value_Raw*)node;

		dat_value_get(value_node, value);
	}
}

Fx_Spike_Params fx_read_spike_params(const Dat_Object* obj)
{
	Fx_Spike_Params params;
	dat_read(obj, "from", &params.from);
	dat_read(obj, "to", &params.to);
	dat_read(obj, "duration", &params.duration);
	dat_read(obj, "size", &params.size);
	dat_read(obj, "center_alpha", &params.center_alpha);

	return params;
}

void fx_make_res(const Fx_Resource* resource, const Fx_Params& params)
{
	// Get emitter array
	const Dat_Array* array = dat_get_array(resource->doc.root, "emitters");
	if (array == nullptr)
		return;

	for(u32 i=0; i<array->size; ++i)
	{
		if (array->elements[i]->type != Dat_Node_Type::Object)
			continue;

		const Dat_Object* object = (const Dat_Object*)array->elements[i];
		const char* emitter_type = nullptr;
		dat_read(object, "type", &emitter_type);

		// No emitter type..
		if (emitter_type == nullptr)
		{
			msg_box("Emitter in fx file doesn't have a type");
			continue;
		}

		if (strcmp(emitter_type, "Spike") == 0)
		{
			Fx_Spike_Params spike = fx_read_spike_params(object);
			spike.base = params;

			debug_log("Making spike (%f, %f)", spike.from, spike.to);
			fx_make_spike(spike);
		}
		else
		{
			msg_box("Unknown emitter type '%s' in fx file", emitter_type);
			continue;
		}
	}
}

#endif