#include "Drawable.h"
#include "Render.h"
#include "Core/Container/SparseList.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/Texture.h"
#include "Runtime/Render/ColorMap.h"

void drawable_init(Drawable* drawable, const Mesh* mesh, const Material* material, const Texture* texture)
{
	drawable->mesh = mesh;
	drawable->material = material;
	drawable->texture = texture;
}

void drawable_render(const Drawable* drawable, const Render_State& state)
{
	const Mesh* mesh = drawable->mesh;
	const Material* material = drawable->material;

	if (drawable->texture != nullptr)
		texture_bind(drawable->texture, 0);

	material_bind(material);
	color_map_bind();

	material_set(material, "u_View", state.view);
	material_set(material, "u_Projection", state.projection);
	material_set(material, "u_ViewProjection", state.view_projection);
	material_set(material, "u_LightViewProjection", state.light_view_projection);
	material_set(material, "u_LightDirection", state.light_direction);
	material_set(material, "u_ColorMapSampler", COLOR_MAP_UNIFORM);
	material_set(material, RENDER_SHADOW_BUFFER_UNIFORM, RENDER_SHADOW_BUFFER_TEXTURE_INDEX);

	material_set(material, "u_Model", drawable->transform);
	mesh_draw(drawable->mesh);
}