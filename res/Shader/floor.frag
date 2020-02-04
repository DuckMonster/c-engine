#include "Shader/Include/ColorMap.frag"
#include "Shader/Include/Lighting.frag"
#include "Shader/Include/Noise.frag"

in vec3 f_World;
in vec2 f_UV;

uniform sampler2D u_ColorMap;
out vec4 o_Color;

void main()
{
	vec4 color_low = color_map_get(COLOR_LOW_X, COLOR_LOW_Y);
	vec4 color_high = color_map_get(COLOR_HIGH_X, COLOR_HIGH_Y);

	float noise = perlin(f_World * 0.1) * 0.5 + 0.5;
	noise = round(noise);
	noise = 1.0;

	vec4 albedo = mix(color_low, color_high, noise);
	vec3 lighted = apply_lighting(albedo.xyz, f_World, vec3(0.0, 0.0, 1.0));

	o_Color = vec4(lighted, 1.0);
} 