#include "Drawable.h"
#include "Render.h"
#include "Core/Container/SparseList.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/Texture.h"

#define MAX_DRAWABLES 1024

namespace
{
	Sparse_List<Drawable> drawable_list;
}

void drawable_init()
{
	splist_create(&drawable_list, MAX_DRAWABLES);
}

Drawable* drawable_make(const Mesh* mesh, const Material* material, const Texture* texture)
{
	assert(mesh != nullptr);
	assert(material != nullptr);

	Drawable* drawable = splist_add(&drawable_list);
	drawable->mesh = mesh;
	drawable->material = material;
	drawable->texture = texture;

	return drawable;
}

Drawable* drawable_load(const char* mesh_path, const char* material_path, const char* texture_path)
{
	assert(mesh_path != nullptr);
	assert(material_path != nullptr);

	Drawable* drawable = splist_add(&drawable_list);
	drawable->mesh = mesh_load(mesh_path);
	drawable->material = material_load(material_path);

	if (texture_path)
		drawable->texture = texture_load(texture_path);

	return drawable;
}

void drawable_destroy(Drawable* drawable)
{
	splist_remove(&drawable_list, drawable);
	drawable->mesh = nullptr;
	drawable->material = nullptr;
	drawable->texture = nullptr;
}

void drawable_render(const Render_State& state)
{
	Drawable* drawable;
	const Material* last_material = nullptr;
	SPLIST_FOREACH(&drawable_list, drawable)
	{
		const Mesh* mesh = drawable->mesh;
		const Material* material = drawable->material;

		if (drawable->texture != nullptr)
			texture_bind(drawable->texture, 0);

		if (material != last_material)
		{
			glBindVertexArray(drawable->mesh->vao);
			glUseProgram(drawable->material->program);

			material_set(material, "u_View", state.view);
			material_set(material, "u_Projection", state.projection);
			material_set(material, "u_ViewProjection", state.view_projection);
			material_set(material, "u_Light", state.light);
			material_set(material, RENDER_SHADOW_BUFFER_UNIFORM, RENDER_SHADOW_BUFFER_TEXTURE_INDEX);
			last_material = material;
		}

		material_set(material, "u_Model", drawable->transform);
		mesh_draw(drawable->mesh);
	}
}