#include "SpriteSheet.h"
#include "Core/Import/Dat.h"
#include "Engine/Resource/Resource.h"
#include "Engine/Resource/HotReload.h"
#include "Engine/Graphics/Texture.h"

static void sprite_sheet_res_create(Resource* resource)
{
	Sprite_Sheet* sheet = (Sprite_Sheet*)resource->ptr;
	if (sheet == nullptr)
	{
		sheet = new Sprite_Sheet();
		resource->ptr = sheet;
	}

	Dat_Document doc;
	if (!dat_load_file(&doc, resource->path))
	{
		error("Failed to load spritesheet file '%s'", resource->path);
	}

	defer { dat_free(&doc); };

	// Read texture source
	const char* src_path;
	if (!dat_read(doc.root, "source", &src_path))
	{
		msg_box("Spritesheet load failed", "Failed to load spritesheet '%s', no source texture specified", resource->path);
		return;
	}
	if (!dat_read(doc.root, "tile.width", &sheet->tile_width))
	{
		msg_box("Spritesheet load failed", "Failed to load spritesheet '%s', tile.width not specified", resource->path);
		return;
	}
	if (!dat_read(doc.root, "tile.height", &sheet->tile_height))
	{
		msg_box("Spritesheet load failed", "Failed to load spritesheet '%s', tile.height not specified", resource->path);
		return;
	}

	sheet->texture = texture_load(src_path);
	resource_add_dependency(resource, src_path);

	dat_read(doc.root, "padding", &sheet->padding);

	sheet->tile_aspect = (float)sheet->tile_width / sheet->tile_height;
	sheet->tile_width_uv = (float)sheet->tile_width / sheet->texture->width;
	sheet->tile_height_uv = (float)sheet->tile_height / sheet->texture->height;
	sheet->tile_padding_x_uv = (float)sheet->padding / sheet->texture->width;
	sheet->tile_padding_y_uv = (float)sheet->padding / sheet->texture->height;
}

static void sprite_sheet_res_destroy(Resource* resource)
{

}

const Sprite_Sheet* sprite_sheet_load(const char* path)
{
	Resource* resource = resource_load(path, sprite_sheet_res_create, sprite_sheet_res_destroy);
	return (Sprite_Sheet*)resource->ptr;
}