#version 330 core

in vec3 a_Position;
in vec2 a_UV;

out vec2 f_TexCoord;

void main()
{
	gl_Position = vec4(a_Position, 1.0);
	f_TexCoord = a_UV;
}