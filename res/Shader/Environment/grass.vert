#include "Shader/Include/Noise.frag"

#define SWAY_AMOUNT_SCALAR 0.7
#define SWAY_SPEED 0.2
#define SWAY_LOC_SCALAR vec3(0.8, 0.8, 0.5)
#define SWAY_X_OFFSET vec3(0.0, 0.0, 0.0)
#define SWAY_Y_OFFSET vec3(2000.0, 5000.0, 2000.0)
#define SWAY_Z_OFFSET vec3(-2000.0, 1500.0, -500.0)

in vec2 a_LocalPosition;
in vec3 a_Origin;

uniform float u_Time;
uniform mat4 u_InvCamera;
uniform mat4 u_ViewProjection;

out vec3 f_World;

void main()
{
	vec3 local_position = vec3(a_LocalPosition, 0.0);

	vec3 world_location = (u_InvCamera * vec4(local_position, 0.0)).xyz;
	world_location = world_location + a_Origin;

	float sway_amount = a_LocalPosition.y * SWAY_AMOUNT_SCALAR;
	vec3 sway_sample_location = a_Origin + vec3(a_LocalPosition.x, 0.0, a_LocalPosition.y);
	sway_sample_location *= SWAY_LOC_SCALAR;

	vec3 time_offset = vec3(0.0, 0.0, -u_Time * SWAY_SPEED);

	vec3 sway;
	sway.x = perlin(sway_sample_location + SWAY_X_OFFSET + time_offset);
	sway.y = perlin(sway_sample_location + SWAY_Y_OFFSET + time_offset);
	sway.z = perlin(sway_sample_location + SWAY_Z_OFFSET + time_offset);
	sway *= sway_amount;

	world_location += sway;

	gl_Position = u_ViewProjection * vec4(world_location, 1.0);
	f_World = world_location;
}