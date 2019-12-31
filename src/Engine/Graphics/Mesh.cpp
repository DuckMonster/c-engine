#include "Mesh.h"
#include "Core/Import/Fbx.h"
#include "Engine/Resource/Resource.h"

void mesh_create(Mesh* mesh)
{
	glGenVertexArrays(1, &mesh->vao);
	mesh->num_buffers = 0;
}

void mesh_add_buffers(Mesh* mesh, u32 count)
{
	assert(mesh->num_buffers + count <= MESH_BUFFER_MAX);
	glGenBuffers(count, mesh->buffers + mesh->num_buffers);

	mesh->num_buffers += count;
}

void mesh_map_buffer(Mesh* mesh, u32 buffer_index, u32 attribute_index, u32 element_count, u32 stride, u32 offset)
{
	glBindVertexArray(mesh->vao);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers[buffer_index]);

	glEnableVertexAttribArray(attribute_index);
	glVertexAttribPointer(attribute_index, element_count, GL_FLOAT, false, stride * sizeof(float), (void*)(offset * sizeof(float)));

	glBindVertexArray(0);
}

void mesh_buffer_data(Mesh* mesh, u32 buffer_index, void* data, u32 size, Mesh_Storage_Type storage_type)
{
	GLuint gl_storage = GL_STATIC_DRAW;
	switch(storage_type)
	{
		case Mesh_Storage_Static: gl_storage = GL_STATIC_DRAW; break;
		case Mesh_Storage_Dynamic: gl_storage = GL_DYNAMIC_DRAW; break;
		case Mesh_Storage_Stream: gl_storage = GL_STREAM_DRAW; break;
	}
	glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers[buffer_index]);
	glBufferData(GL_ARRAY_BUFFER, size, data, gl_storage);
}

void mesh_element_data(Mesh* mesh, u32 buffer_index, void* data, u32 size)
{
	glBindVertexArray(mesh->vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->buffers[buffer_index]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void mesh_free(Mesh* mesh)
{
	glDeleteVertexArrays(1, &mesh->vao);
	glDeleteBuffers(mesh->num_buffers, mesh->buffers);
	mesh->num_buffers = 0;
}

void mesh_bind(const Mesh* mesh)
{
	glBindVertexArray(mesh->vao);
}

void mesh_draw(const Mesh* mesh)
{
	glBindVertexArray(mesh->vao);

	if (mesh->use_elements)
		glDrawElements(mesh->draw_mode, mesh->draw_count, GL_UNSIGNED_INT, NULL);
	else
		glDrawArrays(mesh->draw_mode, 0, mesh->draw_count);

	glBindVertexArray(0);
}

void mesh_load_triangle(Mesh* mesh)
{
	float verts[] = {
		-0.5f, -0.5f, 0.f,
		0.5f, -0.5f, 0.f,
		0.f, 0.5f, 0.f
	};

	mesh_load_verts(mesh, verts, sizeof(verts));
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

	mesh_create(mesh);
	mesh_add_buffers(mesh, 4);
	mesh_map_buffer(mesh, 0, 0, 3, 0, 0);
	mesh_map_buffer(mesh, 1, 1, 3, 0, 0);
	mesh_map_buffer(mesh, 2, 2, 2, 0, 0);

	if (f_mesh.positions != nullptr)
		mesh_buffer_data(mesh, 0, f_mesh.positions, sizeof(Vec3) * f_mesh.num_verts);

	if (f_mesh.normals != nullptr)
		mesh_buffer_data(mesh, 1, f_mesh.normals, sizeof(Vec3) * f_mesh.num_verts);

	if (f_mesh.uvs != nullptr)
		mesh_buffer_data(mesh, 2, f_mesh.uvs, sizeof(Vec2) * f_mesh.num_verts);

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

		mesh_element_data(mesh, 3, indicies, num_triangles * sizeof(u32) * 3);

		mesh->draw_count = num_triangles * 3;
		mesh->use_elements = true;
	}

	fbx_free(scene);
}

void mesh_res_destroy(Resource* resource)
{
	Mesh* mesh = (Mesh*)resource->ptr;
	mesh_free(mesh);
}

const Mesh* mesh_load(const char* path)
{
	// Load resource
	Resource* resource =  resource_load(path, mesh_res_create, mesh_res_destroy);
	return (Mesh*)resource->ptr;
}

void mesh_load_verts(Mesh* mesh, void* v_ptr, u32 size)
{
	mesh_create(mesh);

	mesh_add_buffers(mesh, 1);
	mesh_map_buffer(mesh, 0, 0, 3, 0, 0);
	mesh_buffer_data(mesh, 0, v_ptr, size);

	mesh->num_buffers = 1;
	mesh->draw_count = (size / sizeof(float)) / 3;
}
