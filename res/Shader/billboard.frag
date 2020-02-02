in vec3 f_World;
in vec3 f_Normal;
in vec2 f_UV;

uniform sampler2D u_Sampler;
uniform vec4 u_FillColor = vec4(0.0, 0.0, 0.0, 0.0);

out vec4 o_Color;

void main()
{
	o_Color = texture(u_Sampler, f_UV);
	o_Color.xyz = mix(o_Color.xyz, u_FillColor.xyz, u_FillColor.a);

	if (o_Color.a < 0.5)
		discard;
}