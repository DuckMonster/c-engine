#version 330 core

in vec3 a_Position;
in vec3 a_Normal;

uniform float u_Time;
uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

out vec3 f_Normal;

void main()
{
	// Rotate around Y
	vec3 pos = a_Position;
	float s_time = sin(u_Time);
	float c_time = cos(u_Time);

	pos.x = a_Position.x * c_time - a_Position.z * s_time;
	pos.z = a_Position.z * c_time + a_Position.x * s_time;

	gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
	f_Normal = a_Normal;
}