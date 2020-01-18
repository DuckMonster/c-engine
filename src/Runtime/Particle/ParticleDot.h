#pragma once

struct Particle_Dot_Data
{
	bool enabled;
};

struct Particle_Dot
{
	Vec3 position;
	Vec4 color;
};

void particle_dot_update(Particle_Dot* dots, Particle_Dot_Data* data, u32 num);
void particle_dot_render(Particle_Dot* dots, Particle_Dot_Data* data, u32 num);