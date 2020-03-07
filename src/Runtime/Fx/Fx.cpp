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

void fx_node_get_value_or_range(const Dat_Node* node, float* value)
{
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
			msg_box("Invalid float range in fx file");
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

void fx_read_value_or_range(const Dat_Object* obj, const char* expr, float* value)
{
	const Dat_Node* node = dat_get_node(obj, expr);
	fx_node_get_value_or_range(node, value);
}

void fx_read_vec3(const Dat_Object* root, const char* expr, Vec3* value)
{
	const Dat_Node* value_node = dat_get_node(root, expr);
	if (value_node->type != Dat_Node_Type::Array)
	{
		msg_box("Invalid vec3 '%s' in fx file", expr);
		return;
	}

	const Dat_Array* array = (const Dat_Array*)value_node;
	fx_node_get_value_or_range(array->elements[0], &value->x);
	fx_node_get_value_or_range(array->elements[1], &value->y);
	fx_node_get_value_or_range(array->elements[2], &value->z);
}

Fx_Spike_Params fx_read_spike_params(const Dat_Object* obj)
{
	Fx_Spike_Params params;
	fx_read_value_or_range(obj, "from", &params.from);
	fx_read_value_or_range(obj, "to", &params.to);
	fx_read_value_or_range(obj, "duration", &params.duration);
	fx_read_value_or_range(obj, "size", &params.size);
	fx_read_value_or_range(obj, "center_alpha", &params.center_alpha);

	fx_read_value_or_range(obj, "from_delta", &params.from_delta);
	fx_read_value_or_range(obj, "to_delta", &params.to_delta);

	fx_read_value_or_range(obj, "move_exponent", &params.move_exponent);
	fx_read_value_or_range(obj, "size_exponent", &params.size_exponent);

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

		u32 count = 1;
		dat_read(object, "count", &count);

		for(u32 j=0; j<count; ++j)
		{
			// Transform the direction and origin
			Fx_Params transformed_params = params;

			// Rotation
			Vec3 rotate_euler = Vec3_Zero;
			fx_read_vec3(object, "rotation", &rotate_euler);

			transformed_params.direction =
				quat_x(quat_from_x(params.direction) * quat_from_euler(rotate_euler));

			// Read which type of effect it is
			if (strcmp(emitter_type, "Spike") == 0)
			{
				Fx_Spike_Params spike = fx_read_spike_params(object);
				spike.base = transformed_params;

				fx_make_spike(spike);
			}
			else
			{
				msg_box("Unknown emitter type '%s' in fx file", emitter_type);
				continue;
			}
		}
	}
}

#endif