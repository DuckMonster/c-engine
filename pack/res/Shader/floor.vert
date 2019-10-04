#version 330 core

in vec3 a_Position;

uniform mat4 u_ViewProjection;
out vec3 f_World;

void main()
{
	vec3 pos = a_Position * 100.0;
	gl_Position = u_ViewProjection * vec4(pos, 1.0);
	f_World = pos;
}