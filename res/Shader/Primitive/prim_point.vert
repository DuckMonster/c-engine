#version 330 core
in vec2 a_Position;

uniform mat4 u_PointMatrix;
uniform mat4 u_ViewProjection;

void main()
{
	gl_Position = u_ViewProjection * u_PointMatrix * vec4(a_Position, 0.0, 1.0);
}