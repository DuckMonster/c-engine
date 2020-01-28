#include "Texture.h"
#include "Engine/Resource/Resource.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/Mesh.h"
#include "Core/Import/TGA.h"

void texture_create(Texture* tex)
{
	glGenTextures(1, &tex->handle);
	glBindTexture(GL_TEXTURE_2D, tex->handle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void texture_free(Texture* tex)
{
	glDeleteTextures(1, &tex->handle);
}

void texture_res_create(Resource* resource)
{
	Texture* texture = (Texture*)resource->ptr;
	if (texture == nullptr)
	{
		texture = new Texture();
		resource->ptr = texture;
	}

	texture_create(texture);

	Tga_File tga;
	if (!tga_load(&tga, resource->path))
	{
		msg_box("Texture load failed", "Failed to load TGA file '%s'", resource->path);
		return;
	}

	defer { tga_free(&tga); };

	texture->width = tga.width;
	texture->height = tga.height;

	glBindTexture(GL_TEXTURE_2D, texture->handle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tga.width, tga.height, 0, GL_BGRA, GL_UNSIGNED_BYTE, tga.data);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void texture_res_destroy(Resource* resource)
{
	Texture* texture = (Texture*)resource->ptr;
	texture_free(texture);

	texture->width = 0;
	texture->height = 0;
}

const Texture* texture_load(const char* path)
{
	Resource* resource = resource_load(path, texture_res_create, texture_res_destroy);
	return (Texture*)resource->ptr;
}

void texture_data(const Texture* texture, u32 elements, u32 width, u32 height, const void* data)
{
	GLenum format;
	switch(elements)
	{
		case 1: format = GL_RED; break;
		case 2: format = GL_RG; break;
		case 3: format = GL_RGB; break;
		case 4: format = GL_RGBA; break;
		default:
			error("Unknown number of texture elements %d", elements);
			break;
	}

	glBindTexture(GL_TEXTURE_2D, texture->handle);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void texture_bind(const Texture* texture, u8 index)
{
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, texture != nullptr ? texture->handle : 0);
}

void texture_draw_fullscreen(Texture* tex)
{
	static bool loaded = false;
	static const Material* mat; 
	static const Mesh* mesh;

	if (!loaded)
	{
		mat = material_load("Material/full_quad.mat");
		mesh = mesh_load("Mesh/full_quad.fbx");
		loaded = true;
	}

	glBindVertexArray(mesh->vao);
	glDisable(GL_DEPTH_TEST);
	material_bind(mat);
	texture_bind(tex, 0);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glEnable(GL_DEPTH_TEST);
}