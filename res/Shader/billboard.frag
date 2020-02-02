#define SHADOW_BIAS 0.1

in vec3 f_World;
in vec3 f_Normal;
in vec2 f_UV;

uniform sampler2D u_Sampler;
uniform vec4 u_FillColor = vec4(0.0, 0.0, 0.0, 0.0);

uniform sampler2D u_ShadowSampler;
uniform mat4 u_Light;

out vec4 o_Color;

float sample_shadow(vec3 world)
{
	vec4 light_space = u_Light * vec4(world, 1.0);
	light_space /= light_space.w;

	light_space = light_space * 0.5 + 0.5;

	vec2 light_uv = light_space.xy;
	float self_depth = light_space.z;
	float light_depth = texture(u_ShadowSampler, light_uv).x + SHADOW_BIAS;

	return step(self_depth, light_depth);
}

void main()
{
	o_Color = texture(u_Sampler, f_UV);
	//o_Color.xyz *= sample_shadow(f_World);
	o_Color.xyz = mix(o_Color.xyz, u_FillColor.xyz, u_FillColor.a);

	if (o_Color.a < 0.5)
		discard;
}