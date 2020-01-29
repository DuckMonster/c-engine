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

	for(u32 i=0; i<count; ++i)
	{
		Mesh_Buffer& buffer = mesh->buffers[mesh->num_buffers++];
		mem_zero(&buffer, sizeof(buffer));
		glGenBuffers(1, &buffer.handle);
	}
}

void mesh_add_buffer_mapping(Mesh* mesh, u32 buffer_index, u32 attribute_index, u32 element_count)
{
	glBindVertexArray(mesh->vao);

	Mesh_Buffer& buffer = mesh->buffers[buffer_index];

	glBindBuffer(GL_ARRAY_BUFFER, buffer.handle);

	// Add the new mapping!
	Mesh_Buffer_Mapping& new_mapping = buffer.mapping[buffer.num_mappings++];
	new_mapping.element_count = element_count;
	new_mapping.element_offset = buffer.total_element_count;
	new_mapping.attribute_index = attribute_index;

	buffer.total_element_count += element_count;

	// Enable it
	glEnableVertexAttribArray(attribute_index);

	// Since we've added more elements, we need to update all the strides of the previous mappings
	for(u32 i=0; i<buffer.num_mappings; ++i)
	{
		Mesh_Buffer_Mapping& mapping = buffer.mapping[i];
		glVertexAttribPointer(
			mapping.attribute_index, mapping.element_count, GL_FLOAT, false,
			buffer.total_element_count * sizeof(float),
			(void*)(mapping.element_offset * sizeof(float)));
	}

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
	glBindBuffer(GL_ARRAY_BUFFER, mesh->buffers[buffer_index].handle);
	glBufferData(GL_ARRAY_BUFFER, size, data, gl_storage);

	// Re-set the draw count to the number of floats divided by the total vertex-size of the selected buffer
	if (mesh->buffers[buffer_index].total_element_count > 0)
	{
		u32 num_elements = size / sizeof(float);
		mesh->draw_count = num_elements / mesh->buffers[buffer_index].total_element_count;
	}
}

void mesh_element_data(Mesh* mesh, u32 buffer_index, void* data, u32 size)
{
	glBindVertexArray(mesh->vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->buffers[buffer_index].handle);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	glBindVertexArray(0);

	// Re-set the draw count to the number of indicies
	mesh->draw_count = size / sizeof(u32);
	mesh->use_elements = true;
}

void mesh_free(Mesh* mesh)
{
	glDeleteVertexArrays(1, &mesh->vao);
	for(u32 i=0; i<mesh->num_buffers; ++i)
		glDeleteBuffers(1, &mesh->buffers[i].handle);
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
		return;

	defer { fbx_free(scene); };

	Fbx_Mesh& fbx_mesh = scene->meshes[0];

	mesh_create(mesh);
	mesh_add_buffers(mesh, 2);
	mesh_add_buffer_mapping(mesh, 0, 0, 3); // Positions
	mesh_add_buffer_mapping(mesh, 0, 1, 3); // Normals
	mesh_add_buffer_mapping(mesh, 0, 2, 2); // UVs

	// Build the mapping of all the vertices
	struct Vertex
	{
		Vec3 position;
		Vec3 normal;
		Vec2 uv;
	};

	Vertex* vertex_array = new Vertex[fbx_mesh.num_verts];
	for(u32 i=0; i<fbx_mesh.num_verts; ++i)
	{
		vertex_array[i].position = fbx_mesh.positions[fbx_mesh.position_index[i]];
		if (fbx_mesh.normals)
			vertex_array[i].normal = fbx_mesh.normals[fbx_mesh.normal_index[i]];
		if (fbx_mesh.uvs)
			vertex_array[i].uv = fbx_mesh.uvs[fbx_mesh.uv_index[i]];
	}

	mesh_buffer_data(mesh, 0, vertex_array, sizeof(Vertex) * fbx_mesh.num_verts);

	// Build the elements buffer to split all faces into triangles
	u32 num_triangles = 0;
	for(u32 i=0; i<fbx_mesh.num_faces; ++i)
	{
		// First 3 vertices make one triangle.
		// Each additional vertex makes one more triangle.
		num_triangles += 1 + (fbx_mesh.faces[i].vert_count - 3);
	}

	u32* element_data = new u32[num_triangles * 3];
	u32 element_offset = 0;
	for(u32 i=0; i<fbx_mesh.num_faces; ++i)
	{
		Fbx_Face& face = fbx_mesh.faces[i];
		for(u32 v=2; v<face.vert_count; ++v)
		{
			// Triangle fan: v[0] - v[n-1] - v[n]
			element_data[element_offset++] = face.index_offset;
			element_data[element_offset++] = face.index_offset + v - 1;
			element_data[element_offset++] = face.index_offset + v;
		}
	}

	mesh_element_data(mesh, 1, element_data, sizeof(u32) * num_triangles * 3);
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