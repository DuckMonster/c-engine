#version 330 core

in vec4 f_FragNDC;

uniform sampler2D u_DepthSampler;
uniform vec4 u_Color;
out vec4 o_Color;

void main()
{
	vec4 color = u_Color;

	// Check if we're being "blocked" by something in the main color pass
	// Transform from NDC to texture space
	vec4 ndc = f_FragNDC * 0.5 + 0.5;
	float depth_test = texture(u_DepthSampler, ndc.xy).x;

	// We are...
	if (ndc.z > depth_test)
	{
		// Make it darker
		color.rgb *= 0.4;
	}

	o_Color = color;
}