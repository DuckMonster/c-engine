#include "Shader/Include/Noise.frag"
#include "Shader/Include/FastTrig.gl"

#define SWAY_AMOUNT_SCALAR 0.5
#define SWAY_SPEED 0.2
#define SWAY_LOC_SCALAR vec3(0.8, 0.8, 1.0)
#define SWAY_X_OFFSET vec3(0.0, 0.0, 0.0)
#define SWAY_Y_OFFSET vec3(2000.0, 5000.0, 2000.0)
#define SWAY_Z_OFFSET vec3(-2000.0, 1500.0, -500.0)

in vec2 a_LocalPosition;
in vec3 a_Origin;

uniform float u_Time;
uniform mat4 u_InvCamera;
uniform mat4 u_ViewProjection;

out vec3 f_Origin;
out vec2 f_LocalPosition;
out vec3 f_World;
out vec3 f_Sway;

vec3 sway_perlin()
{
	float sway_amount = a_LocalPosition.y * SWAY_AMOUNT_SCALAR;
	vec3 sway_sample_location = a_Origin + vec3(a_LocalPosition.x, 0.0, a_LocalPosition.y);
	sway_sample_location *= SWAY_LOC_SCALAR;

	vec3 time_offset = vec3(0.0, 0.0, -u_Time * SWAY_SPEED);

	vec3 sway;
	sway.x = perlin(sway_sample_location + SWAY_X_OFFSET + time_offset);
	sway.y = perlin(sway_sample_location + SWAY_Y_OFFSET + time_offset);
	sway.z = perlin(sway_sample_location + SWAY_Z_OFFSET + time_offset);

	return sway * sway_amount;
}

float sway_trig_func(vec3 location)
{
	return (cos(location.x * 2.421) + sin(location.x * 1.241 + location.y * -1.521235) + cos(location.z * -1.9213521 + location.x * 1.135222 + location.y * 1.313523123)) * 0.33;
}

vec3 sway_trig()
{
	float sway_amount = a_LocalPosition.y * SWAY_AMOUNT_SCALAR;
	vec3 sway_sample_location = a_Origin + vec3(a_LocalPosition.x, 0.0, a_LocalPosition.y);
	sway_sample_location *= SWAY_LOC_SCALAR;

	float time_offset = u_Time * SWAY_SPEED * 4.0;

	vec3 sway;
	sway.x = sway_trig_func(sway_sample_location + SWAY_X_OFFSET + vec3(time_offset * 0.9, 0.0, 0.0));
	sway.y = sway_trig_func(sway_sample_location + SWAY_Y_OFFSET + vec3(0.0, time_offset * -0.8, 0.0));
	sway.z = sway_trig_func(sway_sample_location + SWAY_Z_OFFSET + vec3(0.0, 0.0, time_offset * -0.2));

	return sway * sway_amount;
}

void main()
{
	vec3 local_position = vec3(a_LocalPosition, 0.0);

	vec3 world_location = (u_InvCamera * vec4(local_position, 0.0)).xyz;
	world_location = world_location + a_Origin;

	//vec3 sway = sway_perlin();
	vec3 sway = sway_trig();
	f_Sway = sway;
	world_location += sway;

	gl_Position = u_ViewProjection * vec4(world_location, 1.0);
	f_World = world_location;
	f_Origin = a_Origin;
	f_LocalPosition = a_LocalPosition;
}