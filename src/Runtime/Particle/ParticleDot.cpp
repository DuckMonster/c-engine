#include "ParticleDot.h"
#include "Engine/Graphics/Mesh.h"

void particle_dot_update(Particle_Dot* dots, Particle_Dot_Data* data, u32 num)
{
	for(u32 i=0; i<num; ++i)
	{

	}
}

void particle_dot_render(Particle_Dot* dots, Particle_Dot_Data* data, u32 num)
{
	static Mesh dot_mesh;
	do_once
	{
		mesh_create(&dot_mesh);
		mesh_add_buffers(&dot_mesh, 1);

	}
}