in vec2 a_Position;
in vec2 a_UV;

uniform mat4 u_ScreenMatrix;
uniform mat4 u_TextMatrix;

out vec2 f_UV;

void main()
{
	gl_Position = u_ScreenMatrix * u_TextMatrix * vec4(a_Position, 0.0, 1.0);
	f_UV = a_UV;
}