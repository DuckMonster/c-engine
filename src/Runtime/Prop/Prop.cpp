#include "Prop.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include "Runtime/Render/Drawable.h"
#include "Runtime/Game/Scene.h"
#include "Runtime/Prop/PropResource.h"

void prop_init(Prop* prop, const char* path)
{
	Prop_Resource* resource = prop_resource_load(path);

#if CLIENT
	prop->drawable = scene_make_drawable(resource->mesh, resource->material);
#endif
}

void prop_free(Prop* prop)
{
#if CLIENT
	scene_destroy_drawable(prop->drawable);
#endif
}

void prop_set_transform(Prop* prop, const Transform& transform)
{
	prop->transform = transform;

#if CLIENT
	prop->drawable->transform = transform_mat(transform);
#endif
}