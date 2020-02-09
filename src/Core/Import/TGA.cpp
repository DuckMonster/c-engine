#include "TGA.h"
#include <stdio.h>
#include <stdlib.h>

const u8 RUN_LENGTH_BIT = 0b1000;

enum class Tga_Image_Type : u8
{
	NoImageData,
	ColorMapped,
	TrueColor,
	BlackAndWhite
};

#pragma pack(push, 1)

struct Tga_Header
{
	// Length of image id field
	u8 id_length;

	// 0 if no color map, 1 if there is
	u8 color_map_type;

	// Describes the image type (colored, grayscale etc.)
	u8 image_type;
};

struct Tga_Color_Map
{
	u16 first_index;
	u16 length;
	u8 bits_per_pixel;
};

struct Tga_Image_Spec
{
	u16 x_origin;
	u16 y_origin;
	u16 width;
	u16 height;
	u8 pixel_depth;
	u8 image_descriptor;
};

#pragma pack(pop)

bool tga_load(Tga_File* tga, const char* path)
{
	FILE* file = fopen(path, "rb");
	if (file == nullptr)
	{
		debug_log("Failed to load TGA file '%s', file doesn't exist", path);
		return false;
	}

	defer { fclose(file); };

	// Read header
	Tga_Header header;
	Tga_Color_Map color_map;
	Tga_Image_Spec image_spec;

	fread(&header, sizeof(header), 1, file);
	fread(&color_map, sizeof(color_map), 1, file);
	fread(&image_spec, sizeof(image_spec), 1, file);

	// Read image ID
	fread(nullptr, header.id_length, 1, file);

	// Read pixel data
	tga->width = image_spec.width;
	tga->height = image_spec.height;
	tga->channels = image_spec.pixel_depth / 8;

	u32 image_size = image_spec.width * image_spec.height * tga->channels;
	tga->data = malloc(image_size);
	fread(tga->data, image_size, 1, file);

	return true;
}

void tga_free(Tga_File* tga)
{
	if (tga->data != nullptr)
	{
		delete tga->data;
		tga->data = nullptr;
	}

	tga->width = 0;
	tga->height = 0;
}
