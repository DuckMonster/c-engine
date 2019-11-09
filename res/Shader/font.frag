#version 330 core

uniform sampler2D u_Sampler;

in vec2 f_UV;
out vec4 o_Color;

void main()
{
	o_Color = texture(u_Sampler, f_UV);
}