#include "Shader/Include/ColorMap.frag"

in VertData
{
	vec3 normal;
	vec2 uv;
} v2f;

uniform vec3 u_LightDirection;

in vec3 f_Normal;
out vec4 o_Color;

void main()
{
	vec3 color = vec3(0.6, 0.3, 0.2);
	float diffuse = -dot(v2f.normal, u_LightDirection);
	diffuse = clamp(diffuse, 0.2, 1.0);

	o_Color = vec4(MY_VALUE, MY_OTHER_VALUE, 0.0, 1.0);
}