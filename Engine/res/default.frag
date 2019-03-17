#version 330 core

out vec4 o_Color;
in vec3 f_Normal;

void main()
{
	o_Color = vec4(f_Normal, 1.0);
}