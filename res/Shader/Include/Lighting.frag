#include "Shader/Include/ColorMap.frag"
#define SHADOW_BIAS 0.005

uniform sampler2D u_ShadowBuffer;
uniform mat4 u_LightViewProjection;

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