#version 330 core

in vec2 a_Position;
in vec2 a_UV;

out vec2 f_UV;

void main()
{
	gl_Position = vec4(a_Position, 0.0, 1.0);
	f_UV = a_UV;
}