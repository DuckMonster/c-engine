in vec3 a_Position;
out vec4 f_FragNDC;

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;

void main()
{
	gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
	f_FragNDC = gl_Position;
	f_FragNDC /= f_FragNDC.w;
}