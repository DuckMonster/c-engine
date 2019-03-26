#version 330 core
uniform sampler2D u_Sampler;

in vec2 f_TexCoord;
out vec4 o_Color;

void main()
{
	o_Color = texture(u_Sampler, f_TexCoord);
	//o_Color = vec4(f_TexCoord, 0.0, 1.0);
}