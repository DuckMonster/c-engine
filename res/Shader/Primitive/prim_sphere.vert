#version 330 core
in vec3 a_Position;

uniform mat4 u_SphereMatrix;
uniform mat4 u_ViewProjection;

void main()
{
	gl_Position = u_ViewProjection * u_SphereMatrix * vec4(a_Position, 1.0);
}