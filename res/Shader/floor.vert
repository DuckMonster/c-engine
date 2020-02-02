in vec3 a_Position;
layout(location = 2) in vec2 a_UV;

uniform mat4 u_ViewProjection;
out vec3 f_World;
out vec2 f_UV;

void main()
{
	vec3 pos = a_Position * 100.0;
	gl_Position = u_ViewProjection * vec4(pos, 1.0);
	f_World = pos;
	f_UV = a_UV;
}