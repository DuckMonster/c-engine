#version 330 core
in vec2 a_Position;

uniform mat4 u_LineMatrix;
uniform mat4 u_ViewProjection;

void main()
{
	gl_Position = u_ViewProjection * u_LineMatrix * vec4(a_Position, 0.0, 1.0);
}