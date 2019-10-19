#version 330 core

uniform sampler2D u_Sampler;

out vec4 o_Color;
in vec3 f_Normal;
in vec2 f_UV;

void main()
{
	o_Color = vec4(f_Normal, 1.0);
	//o_Color = texture(u_Sampler, f_UV);
}