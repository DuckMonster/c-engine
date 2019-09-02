#version 330 core

in vec3 a_Position;
in vec3 a_Normal;
in vec2 a_UV;

uniform float u_Time;
uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

out vec3 f_Normal;
out vec2 f_UV;

void main()
{
	gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
	f_Normal = (u_Model * vec4(a_Normal, 0.0)).xyz;
	f_UV = a_UV;
}