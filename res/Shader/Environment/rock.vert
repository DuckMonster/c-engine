in vec3 a_Position;
in vec3 a_Normal;
in vec2 a_UV;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

out VertData
{
	vec3 world;
	vec3 normal;
	vec2 uv;
} v2f;

void main()
{
	vec4 world = u_Model * vec4(a_Position, 1.0);

	gl_Position = u_ViewProjection * world;
	v2f.world = world.xyz;
	v2f.normal = normalize((u_Model * vec4(a_Normal, 0.0)).xyz);
	v2f.uv = a_UV;
}