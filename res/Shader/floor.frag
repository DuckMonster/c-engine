#include "Shader/Include/Lighting.frag"

in vec3 f_World;
in vec2 f_UV;

uniform sampler2D u_ColorMap;
out vec4 o_Color;

void main()
{
	int pos = int(floor(f_World.x) + floor(f_World.y));
	float grad = pos % 2 == 0 ? 0.15 : 0.1;

	o_Color = texture(u_ColorMap, f_UV);
	o_Color.xyz *= sample_shadow(f_World);
} 