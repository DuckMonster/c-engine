uniform sampler2D u_Sampler;

in vec2 f_UV;
out vec4 o_Color;

void main()
{
	float glyph = texture(u_Sampler, f_UV).x;
	if (glyph < 0.5)
		discard;

	o_Color = vec4(glyph, glyph, glyph, 1.0);
}