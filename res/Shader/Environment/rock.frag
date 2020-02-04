#include "Shader/Include/Lighting.frag"
#include "Shader/Include/ColorMap.frag"

in VertData
{
	vec3 world;
	vec3 normal;
	vec2 uv;
} v2f;

out vec4 o_Color;

void main()
{
	vec3 albedo = vec3(0.6, 0.3, 0.2);
	vec3 lighted = apply_lighting(albedo, v2f.world, v2f.normal);

	o_Color = vec4(lighted, 1.0);
}