#include "Shader/Include/ColorMap.frag"
#include "Shader/Include/Lighting.frag"

in vec3 f_Origin;
in vec2 f_LocalPosition;
in vec3 f_World;
in vec3 f_Sway;
out vec4 o_Color;

void main()
{
	vec3 shadow_sample_location = f_Origin + vec3(f_LocalPosition.x, 0.0, f_LocalPosition.y) * 0.2;
	vec4 color = color_map_get(COLOR_X, COLOR_Y);
	vec3 lighted = apply_lighting(color.xyz, f_World, vec3(0.0, 0.0, 1.0));
	o_Color = vec4(lighted, 1.0);
	//o_Color = vec4(f_Sway * 0.5 + 0.5, 1.0);
}