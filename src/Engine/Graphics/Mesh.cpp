#include "Mesh.h"
#include "Engine/Graphics/MeshResource.h"

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

	Mesh_Buffer& buffer = mesh->buffers[buffer_index];
	glBindBuffer(GL_ARRAY_BUFFER, buffer.handle);

	// Same size, just sub-data
	if (buffer.size == size)
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	else
		glBufferData(GL_ARRAY_BUFFER, size, data, gl_storage);

	buffer.size = size;

	// Re-set the draw count to the number of floats divided by the total vertex-size of the selected buffer
	if (buffer.total_element_count > 0)
	{
		u32 num_elements = size / sizeof(float);
		mesh->draw_count = num_elements / buffer.total_element_count;
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

const Mesh* mesh_load(const char* path)
{
	// Load mesh resource
	Mesh_Resource* resource = mesh_resource_load(path);
	return &resource->mesh;
}