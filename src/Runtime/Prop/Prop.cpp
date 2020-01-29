#include "Prop.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include "Runtime/Render/Drawable.h"
#include "Runtime/Game/Scene.h"

void prop_init(Prop* prop)
{
#if CLIENT
	prop->drawable = scene_make_drawable(mesh_load("Mesh/cube.fbx"), material_load("Material/default.mat"));
#endif
}

void prop_free(Prop* prop)
{
	scene_destroy_drawable(prop->drawable);
}

void prop_set_transform(Prop* prop, const Transform& transform)
{
	prop->transform = transform;

#if CLIENT
	prop->drawable->transform = transform_mat(transform);
#endif
}