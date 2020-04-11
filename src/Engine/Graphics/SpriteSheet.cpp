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
		msg_box("Failed to load spritesheet file '%s'", resource->path);
		return;
	}

	defer { dat_free(&doc); };

	// Read texture source
	const char* src_path;
	if (!dat_read(doc.root, "source", &src_path))
	{
		msg_box("Failed to load spritesheet '%s', no source texture specified", resource->path);
		return;
	}
	if (!dat_read(doc.root, "tile.width", &sheet->tile_width))
	{
		msg_box("Failed to load spritesheet '%s', tile.width not specified", resource->path);
		return;
	}
	if (!dat_read(doc.root, "tile.height", &sheet->tile_height))
	{
		msg_box("Failed to load spritesheet '%s', tile.height not specified", resource->path);
		return;
	}

	sheet->texture = texture_load(src_path);
	Resource* tex_res = resource_from_data(sheet->texture);
	resource_add_dependency(resource, tex_res);

	dat_read(doc.root, "padding", &sheet->padding);

	// Save some neat nice variables
	sheet->tile_aspect = (float)sheet->tile_width / sheet->tile_height;
	sheet->tile_width_uv = (float)sheet->tile_width / sheet->texture->width;
	sheet->tile_height_uv = (float)sheet->tile_height / sheet->texture->height;
	sheet->tile_padding_x_uv = (float)sheet->padding / sheet->texture->width;
	sheet->tile_padding_y_uv = (float)sheet->padding / sheet->texture->height;

	// Read animations
	const Dat_Object* anim_root = dat_get_object(doc.root, "animations");
	if (anim_root && anim_root->first_key)
	{
		// First count how many animations there are
		const Dat_Key* key = anim_root->first_key;
		while(key)
		{
			if (key->value->type != Dat_Node_Type::Object)
			{
				msg_box("Error parsing animations in sprite sheet '%s'", resource->path);
				return;
			}
			sheet->num_animations++;
			key = key->next;
		}

		// Okay, now _actually_ read them
		key = anim_root->first_key;
		sheet->animations = new Sprite_Anim[sheet->num_animations];

		Sprite_Anim* anim = sheet->animations;
		while(key)
		{
			const Dat_Object* anim_node = (const Dat_Object*)key->value;

			anim->name = strcpy_malloc(key->name, key->name_len);
			dat_read(anim_node, "origin[0]", &anim->origin_x);
			dat_read(anim_node, "origin[1]", &anim->origin_y);
			dat_read(anim_node, "length", &anim->length);
			dat_read(anim_node, "duration", &anim->duration);

			key = key->next;
			anim++;
		}
	}
}

static void sprite_sheet_res_destroy(Resource* resource)
{

}

const Sprite_Sheet* sprite_sheet_load(const char* path)
{
	return resource_load_t(Sprite_Sheet, path, sprite_sheet_res_create, sprite_sheet_res_destroy);
}

const Sprite_Anim* sprite_sheet_get_animation(const Sprite_Sheet* sheet, const char* name)
{
	for(u32 i=0; i<sheet->num_animations; ++i)
	{
		if (strcmp(sheet->animations[i].name, name) == 0)
			return &sheet->animations[i];
	}

	return nullptr;
}