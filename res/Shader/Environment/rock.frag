#include "Shader/Include/Lighting.frag"

in VertData
{
	vec3 world;
	vec3 normal;
	vec2 uv;
} v2f;

uniform vec3 u_LightDirection;
out vec4 o_Color;

void main()
{
	vec3 color = vec3(0.6, 0.3, 0.2);
	float diffuse = -dot(v2f.normal, u_LightDirection);
	float shadow = sample_shadow(v2f.world);

	diffuse *= shadow;
	diffuse = clamp(diffuse, 0.2, 1.0);

	o_Color = vec4(color * diffuse * shadow, 1.0);
}