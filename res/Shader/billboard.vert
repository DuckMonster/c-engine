#version 330 core

in vec3 a_Position;
in vec3 a_Normal;
in vec2 a_UV;

uniform float u_Time;
uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform mat4 u_AlignMatrix;

uniform mat4 u_TileMatrix;
uniform float u_ScreenWidth;
uniform float u_ScreenHeight;

out vec3 f_World;
out vec3 f_Normal;
out vec2 f_UV;

void main()
{
	vec4 depth_sample = u_Projection * u_View * u_Model * vec4(0.f, -0.5f, 0.f, 1.f);
	gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
	gl_Position.z = depth_sample.z;

	f_World = (u_Model * vec4(a_Position, 1.0)).xyz;
	f_Normal = (u_Model * vec4(a_Normal, 0.0)).xyz;
	f_UV = a_UV;
	f_UV = (u_TileMatrix * vec4(a_UV, 0.0, 1.0)).xy;
}