#include "Shader/Include/ColorMap.frag"
#define SHADOW_BIAS 0.005
#define SHADOW_COLOR_X 0
#define SHADOW_COLOR_Y 3
#define SHADOW_STRENGTH 0.8

#define AMBIENT 0.1

uniform sampler2D u_ShadowBuffer;
uniform mat4 u_LightViewProjection;
uniform vec3 u_LightDirection;

float calc_shadow(vec3 world)
{
	vec4 light_space = u_LightViewProjection * vec4(world, 1.0);
	light_space /= light_space.w;

	light_space = light_space * 0.5 + 0.5;

	vec2 light_uv = light_space.xy;
	float self_depth = light_space.z;
	float light_depth = texture(u_ShadowBuffer, light_uv).x;

	return step(self_depth - SHADOW_BIAS, light_depth);
}

float calc_diffuse(vec3 normal)
{
	float diffuse = clamp(-dot(normal, u_LightDirection), 0.0, 1.0);
	diffuse = step(0.2, diffuse);

	return diffuse;
}

vec3 apply_lighting(vec3 albedo, vec3 world, vec3 normal)
{
	float ambient = AMBIENT;
	float diffuse = calc_diffuse(normal);
	float shadow = calc_shadow(world);
	diffuse *= shadow;

	vec4 shadow_color = color_map_get(SHADOW_COLOR_X, SHADOW_COLOR_Y);
	vec3 color = albedo * (ambient + diffuse);
	color += shadow_color.xyz * (1.0 - diffuse) * SHADOW_STRENGTH;

	return color;
}