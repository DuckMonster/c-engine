#version 330 core

in vec3 a_Position;
in float a_Time;
in float a_Width;

out VS_OUT {
	float time;
	float width;
} vs_out;

void main()
{
	gl_Position = vec4(a_Position, 1.0);
	vs_out.time = a_Time;
	vs_out.width = a_Width;
} 