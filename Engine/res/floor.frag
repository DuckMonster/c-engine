#version 330 core

in vec3 f_World;
out vec4 o_Color;

void main()
{
	int pos = int(floor(f_World.x) + floor(f_World.y));
	float grad = pos % 2 == 0 ? 0.15 : 0.1;
	o_Color = vec4(vec3(grad), 1.0);
} 