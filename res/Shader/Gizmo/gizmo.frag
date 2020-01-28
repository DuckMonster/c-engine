#version 330 core

uniform vec4 u_Color;
out vec4 o_Color;

void main()
{
	o_Color = u_Color;
}