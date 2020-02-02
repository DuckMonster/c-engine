in vec3 a_Position;
in vec3 a_Color;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

out vec3 f_Color;

void main()
{
	gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
	f_Color = a_Color;
}