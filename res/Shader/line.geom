layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

in VS_OUT {
	float time;
	float width;
} vs_out[];

uniform mat4 u_ViewProjection;

void main()
{
	vec3 from = gl_in[0].gl_Position.xyz;
	vec3 to = gl_in[1].gl_Position.xyz;
	float from_size = vs_out[0].width * max(1.f - vs_out[0].time * 2.5, 0.0);
	float to_size = vs_out[1].width * max(1.f - vs_out[1].time * 2.5, 0.0);// / (1.f + vs_out[1].time * 5.0));

	vec3 dir = to - from;
	vec3 right = normalize(cross(dir, vec3(0.0, 0.0, 1.0)));

	gl_Position = u_ViewProjection * vec4(from + right * from_size, 1.0);
	EmitVertex();
	gl_Position = u_ViewProjection * vec4(from - right * from_size, 1.0);
	EmitVertex();
	gl_Position = u_ViewProjection * vec4(to + right * to_size, 1.0);
	EmitVertex();
	gl_Position = u_ViewProjection * vec4(to - right * to_size, 1.0);
	EmitVertex();

	EndPrimitive();
}