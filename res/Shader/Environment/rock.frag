#include "Shader/Include/Lighting.frag"
#include "Shader/Include/ColorMap.frag"
uniform sampler2D u_ColorMask;

in VertData
{
	vec3 world;
	vec3 normal;
	vec2 uv;
} v2f;

out vec4 o_Color;

void main()
{
	float z_mask = clamp(v2f.world.z, 0.0, 1.0);

	vec4 mask_value = texture(u_ColorMask, v2f.uv);
	//mask_value *= z_mask;

	vec3 color_min = color_map_get(COLOR_MIN_X, COLOR_MIN_Y).xyz;
	vec3 color_max = color_map_get(COLOR_MAX_X, COLOR_MAX_Y).xyz;

	vec3 albedo = mix(color_min, color_max, mask_value.x);
	vec3 lighted = apply_lighting(albedo, v2f.world, v2f.normal);

	o_Color = vec4(lighted, 1.0);
}