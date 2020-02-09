#include "Shader/Include/ColorMap.frag"
#include "Shader/Include/Lighting.frag"

in vec3 f_World;
out vec4 o_Color;

void main()
{
	vec4 color = color_map_get(COLOR_X, COLOR_Y);
	vec3 lighted = apply_lighting(color.xyz, f_World, vec3(0.0, 0.0, 1.0));
	o_Color = vec4(lighted, 1.0);
}