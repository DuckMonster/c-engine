#version 330 core
#define SHADOW_BIAS 0.001

in vec3 f_World;

uniform sampler2D u_ShadowBuffer;
uniform mat4 u_LightViewProjection;

out vec4 o_Color;

float sample_shadow(vec3 world)
{
	vec4 light_space = u_LightViewProjection * vec4(world, 1.0);
	light_space /= light_space.w;

	light_space = light_space * 0.5 + 0.5;

	vec2 light_uv = light_space.xy;
	float self_depth = light_space.z;
	float light_depth = texture(u_ShadowBuffer, light_uv).x;

	return step(self_depth - SHADOW_BIAS, light_depth);
}

void main()
{
	int pos = int(floor(f_World.x) + floor(f_World.y));
	float grad = pos % 2 == 0 ? 0.15 : 0.1;

	o_Color = vec4(vec3(grad), 1.0);
	o_Color.xyz *= sample_shadow(f_World);
} 