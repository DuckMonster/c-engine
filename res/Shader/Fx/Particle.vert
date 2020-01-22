#version 330 core

in vec3 a_Position;
in vec4 a_Color;

uniform mat4 u_ViewProjection;

out vec4 f_Color;

void main()
{
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
	f_Color = a_Color;
}