#include "Mesh.h"
#include "Core/Import/Fbx.h"
#include "Engine/Resource/Resource.h"

void mesh_load_triangle(Mesh* mesh)
{
	glGenVertexArrays(1, &mesh->vao);
	glBindVertexArray(mesh->vao);

	float verts[] = {
		-0.5f, -0.5f, 0.f,
		0.5f, -0.5f, 0.f,
		0.f, 0.5f, 0.f
	};

	glGenBuffers(1, mesh->buffers);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);

	glBindVertexArray(0);
	mesh->num_buffers = 1;
	mesh->draw_count = 3;
}

void mesh_res_create(Resource* resource)
{
	Mesh* mesh = (Mesh*)resource->ptr;
	if (mesh == nullptr)
	{
		mesh = new Mesh();
		resource->ptr = mesh;
	}

	Fbx_Scene* scene = fbx_import(resource->path);
	if (scene == nullptr)
	{
		return;
	}

	Fbx_Mesh& f_mesh = scene->meshes[0];

	glGenVertexArrays(1, &mesh->vao);
	glBindVertexArray(mesh->vao);

	if (f_mesh.positions != nullptr)
	{
		GLuint& vbo = mesh->buffers[mesh->num_buffers++];
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3) * f_mesh.num_verts, f_mesh.positions, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	}

	if (f_mesh.normals != nullptr)
	{
		GLuint& vbo = mesh->buffers[mesh->num_buffers++];
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3) * f_mesh.num_verts, f_mesh.normals, GL_STATIC_DRAW);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, 0);
	}

	if (f_mesh.uvs != nullptr)
	{
		GLuint& vbo = mesh->buffers[mesh->num_buffers++];
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vec2) * f_mesh.num_verts, f_mesh.uvs, GL_STATIC_DRAW);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, false, 0, 0);
	}

	{
		// Count how many triangles we have
		u32 num_triangles = 0;
		for(u32 i=0; i<f_mesh.num_faces; ++i)
		{
			Fbx_Face& face = f_mesh.faces[i];
			num_triangles += 1 + (face.vert_count - 3);
		}

		u32* indicies = arena_malloc_t(&scene->mem_arena, u32, num_triangles * 3);

		u32 i=0;
		for(u32 f=0; f<f_mesh.num_faces; ++f)
		{
			Fbx_Face& face = f_mesh.faces[f];

			for(u32 t=0; t<1 + (face.vert_count - 3); ++t)
			{
				indicies[i] = f_mesh.indicies[face.index_offset];
				indicies[i + 1] = f_mesh.indicies[face.index_offset + t + 1];
				indicies[i + 2] = f_mesh.indicies[face.index_offset + t + 2];

				i += 3;
			}
		}

		GLuint& vbo = mesh->buffers[mesh->num_buffers++];
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * num_triangles * 3, indicies, GL_STATIC_DRAW);

		mesh->draw_count = num_triangles * 3;
		mesh->use_elements = true;
	}

	fbx_free(scene);
	glBindVertexArray(0);
}

void mesh_res_destroy(Resource* resource)
{
	Mesh* mesh = (Mesh*)resource->ptr;
	glDeleteVertexArrays(1, &mesh->vao);
	glDeleteBuffers(mesh->num_buffers, mesh->buffers);

	mesh->num_buffers = 0;
	mesh->use_elements = false;
	mesh->draw_count = 0;
}

const Mesh* mesh_load(const char* path)
{
	// Load resource
	Resource* resource =  resource_load(path, mesh_res_create, mesh_res_destroy);
	return (Mesh*)resource->ptr;
}

void mesh_load_verts(Mesh* mesh, void* v_ptr, u32 size)
{
	glGenVertexArrays(1, &mesh->vao);
	glBindVertexArray(mesh->vao);

	glGenBuffers(1, mesh->buffers);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, size, v_ptr, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);

	glBindVertexArray(0);
	mesh->num_buffers = 1;
	mesh->draw_count = (size / sizeof(float)) / 3;
}

void mesh_free(Mesh* mesh)
{
	glDeleteVertexArrays(1, &mesh->vao);
	glDeleteBuffers(mesh->num_buffers, mesh->buffers);
}