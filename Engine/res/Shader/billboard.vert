#version 330 core

in vec3 a_Position;
in vec3 a_Normal;
in vec2 a_UV;

uniform float u_Time;
uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform mat4 u_TileMatrix;

out vec3 f_World;
out vec3 f_Normal;
out vec2 f_UV;

void main()
{
	mat4 view = u_View * u_Model;

	view[0].xyz = vec3(length(u_Model[0]), 0.0, 0.0);
	view[1].xyz = vec3(0.0, length(u_Model[1]), 0.0);
	view[2].xyz = vec3(0.0, 0.0, length(u_Model[2]));

	mat4 view_world = inverse(u_View) * view;

	gl_Position = u_Projection * view * vec4(a_Position, 1.0);

	f_World = (view_world * vec4(a_Position, 1.0)).xyz;
	f_Normal = (u_Model * vec4(a_Normal, 0.0)).xyz;
	f_UV = a_UV;
	f_UV = (u_TileMatrix * vec4(a_UV, 0.0, 1.0)).xy;
}