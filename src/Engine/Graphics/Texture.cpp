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
		msg_box("Failed to load TGA file '%s'", resource->path);
		return;
	}

	defer{ tga_free(&tga); };

	GLenum gl_tex_mode;
	GLenum gl_int_mode;
	switch(tga.channels)
	{
		case 1:
			gl_tex_mode = GL_RED;
			gl_int_mode = GL_RED;
			break;
		case 2:
			gl_tex_mode = GL_RG;
			gl_int_mode = GL_RG;
			break;
		case 3:
			gl_tex_mode = GL_BGR;
			gl_int_mode = GL_RGB;
			break;
		case 4:
			gl_tex_mode = GL_BGRA;
			gl_int_mode = GL_RGBA;
			break;
		default:
			error("Unknown number of channels %d in texture '%s'", tga.channels, resource->path);
			break;
	}

	texture->width = tga.width;
	texture->height = tga.height;

	glBindTexture(GL_TEXTURE_2D, texture->handle);
	glTexImage2D(GL_TEXTURE_2D, 0, gl_int_mode, tga.width, tga.height, 0, gl_tex_mode, GL_UNSIGNED_BYTE, tga.data);
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

void texture_data(Texture* texture, u32 elements, u32 width, u32 height, const void* data)
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

	texture->width = width;
	texture->height = height;
	texture->pixel_size = elements;

	// If there's data here, also copy it onto our own data buffer
	if (data)
	{
		texture->data = malloc(width * height * elements);
		memcpy(texture->data, data, width * height * elements);
	}
}

void texture_bind(const Texture* texture, u8 index)
{
	glActiveTexture(GL_TEXTURE0 + index);
	glBindTexture(GL_TEXTURE_2D, texture != nullptr ? texture->handle : 0);
}

Color_32 texture_get_pixel(const Texture* texture, u32 x, u32 y)
{
	// Invalid size
	if (texture->width == 0 || texture->height == 0)
		return Color_Black_32;

	// Invalid coordinates
	if (x >= texture->width || y >= texture->height)
		return Color_Black_32;

	Color_32 result = color_hex(0);

	if (texture->data != nullptr)
	{
		// We have saved the data on the CPU, so just get it from there
		u32 index = y * texture->width + x;

		memcpy(&result, (u8*)texture->data + index * texture->pixel_size, texture->pixel_size);
		return result;
	}
	else
	{
		// We don't have it saved, so we have to ask the GPU
		// Get the GL-format for the texture
		GLenum format;
		switch(texture->pixel_size)
		{
			case 1: format = GL_RED; break;
			case 2: format = GL_RG; break;
			case 3: format = GL_RGB; break;
			case 4: format = GL_RGBA; break;
			default: return result; // Invalid element count
		}

		glReadPixels(x, y, 1, 1, format, GL_UNSIGNED_BYTE, &result);
		return result;
	}
}

Color_32 texture_get_pixel(const Texture* texture, const Vec2& uv)
{
	// Invalid size
	if (texture->width == 0 || texture->height == 0)
		return Color_Black_32;

	u32 x = (u32)(uv.x * texture->width);
	u32 y = (u32)(uv.y * texture->height);

	return texture_get_pixel(texture, x, y);
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