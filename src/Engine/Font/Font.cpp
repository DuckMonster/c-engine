#include "Font.h"
#include "Engine/Resource/Resource.h"
#include <Engine/Graphics/Mesh.h>
#include <Engine/Graphics/Material.h>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace
{
	FT_Library ft_library;

	void ft_init()
	{
		static bool is_init = false;
		if (is_init)
			return;

		FT_Init_FreeType(&ft_library);
		is_init = true;
	}
}

struct Glyph_Packer
{
	Font* font = nullptr;
	FT_Face face;
	u8* buffer = nullptr;

	// Height buffer keeps track of the highest Y value written to for any X value,
	//		for efficient(-ish) packing!
	u32* height_buffer = nullptr;
	u32 x;
	u32 width;
	u32 height;
};

void glyph_packer_make(Glyph_Packer* packer, Font* font, FT_Face face)
{
	u32 size = 256;
	packer->font = font;
	packer->face = face;
	packer->width = size;
	packer->height = size;
	packer->buffer = (u8*)malloc(size * size);
	packer->height_buffer = (u32*)malloc(size * sizeof(u32));
	packer->x = 0;

	mem_zero(packer->buffer, size * size);
	mem_zero(packer->height_buffer, size * sizeof(u32));
}

void glyph_packer_free(Glyph_Packer* packer)
{
	free(packer->buffer);
	free(packer->height_buffer);
	mem_zero(packer, sizeof(Glyph_Packer));
}

void glyph_pack(Glyph_Packer* packer, char glyph_char)
{
	u32 glyph_index = FT_Get_Char_Index(packer->face, glyph_char);
	FT_Load_Glyph(packer->face, glyph_index, FT_LOAD_DEFAULT);
	FT_Render_Glyph(packer->face->glyph, FT_RENDER_MODE_NORMAL);

	const FT_Bitmap& bitmap = packer->face->glyph->bitmap;

	// Wrap packer x around, if we reach the border
	if (packer->x + bitmap.width > packer->width)
		packer->x = 0;

	// Find the lowest Y we can copy to without writing over any other glyphs
	u32 y = 0;
	for(u32 x=0; x<bitmap.width; ++x)
	{
		if (packer->height_buffer[packer->x + x] > y)
			y = packer->height_buffer[packer->x + x];
	}

	if (y + bitmap.rows > packer->height)
		error("Couldn't fit glyph '%c' on atlas (%u > %u)", glyph_char, y + bitmap.rows, packer->height);

	// Pointer to bottom-left buffer square we're writing to!
	u8* packer_offset =
		packer->buffer + packer->x +
		y * packer->width;

	// Copy pixels!!
	for(u32 y=0; y<bitmap.rows; ++y)
	{
		// Since OpenGL reads textures upside down, flip the glyph bitmap!
		u32 flip_y = bitmap.rows - y - 1;
		memcpy(packer_offset + packer->width * y, bitmap.buffer + bitmap.width * flip_y, bitmap.width);
	}

	// Write to the height buffer
	for(u32 x=0; x<bitmap.width; ++x)
	{
		packer->height_buffer[packer->x + x] = y + bitmap.rows + 1;
	}

	packer->x += bitmap.width + 1;
}

void glyph_pack_range(Glyph_Packer* packer, char start, char end)
{
	for(char c = start; c <= end; ++c)
		glyph_pack(packer, c);
}

void font_res_create(Resource* resource, Font* font)
{
	ft_init();

	FT_Face face;
	FT_New_Face(ft_library, resource->path, 0, &face);
	FT_Set_Pixel_Sizes(face, 0, 21);

	Glyph_Packer packer;
	glyph_packer_make(&packer, font, face);
	defer { glyph_packer_free(&packer); };

	glyph_pack_range(&packer, 32, 126);

	texture_create(&font->atlas);
	texture_data(&font->atlas, 1, packer.width, packer.height, packer.buffer);

	glyph_packer_free(&packer);
}

void font_res_destroy(Resource* resource, Font* font)
{
	texture_free(&font->atlas);
}

const Font* font_load(const char* path)
{
	return resource_load_t(Font, path, font_res_create, font_res_destroy);
}

void font_debug_draw(const Font* font)
{
	static bool is_init = false;
	static Mesh mesh;
	static const Material* material;
	static float mesh_verts[] = {
		-0.5f, -0.5f,	0.0f, 0.0f,
		+0.5f, -0.5f,	1.0f, 0.0f,
		+0.5f, 0.5f,	1.0f, 1.0f,

		-0.5f, -0.5f,	0.0f, 0.0f,
		+0.5f, 0.5f,	1.0f, 1.0f,
		-0.5f, 0.5f,	0.0f, 1.0f,
	};

	if (!is_init)
	{
		material = material_load("Material/font.mat");
		mesh_create(&mesh);
		mesh_add_buffers(&mesh, 1);
		mesh_add_buffer_mapping(&mesh, 0, 0, 2);
		mesh_add_buffer_mapping(&mesh, 0, 1, 2);
		mesh_buffer_data(&mesh, 0, mesh_verts, sizeof(mesh_verts));
	}

	texture_bind(&font->atlas, 0);
	material_bind(material);
	mesh_draw(&mesh);
}