#version 330

in vec3 a_Position;
in vec3 a_Normal;
layout(location = 2) in vec2 a_UV;
out vec2 f_UV;

void main()
{
	gl_Position = vec4(a_Position, 1.0);
	f_UV = a_UV;
};
