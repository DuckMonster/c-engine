#include "PropResource.h"
#include "Core/Import/Dat.h"
#include "Engine/Resource/Resource.h"
#include "Engine/Resource/HotReload.h"
#include "Engine/Graphics/MeshResource.h"
#include "Engine/Graphics/Material.h"

void prop_res_create(Resource* resource, Prop_Resource* prop)
{
	Dat_Document doc;
	if (!dat_load_file(&doc, resource->path))
	{
		msg_box("Failed to load prop file '%s'", resource->path);
		return;
	}

	defer { dat_free(&doc); };

	const char* mesh_path;
	const char* material_path;

	if (!dat_read(doc.root, "mesh", &mesh_path))
	{
		msg_box("No mesh specified");
		return;
	}
	if (!dat_read(doc.root, "material", &material_path))
	{
		msg_box("No material specified");
		return;
	}

	Mesh_Resource* mesh_resource = mesh_resource_load(mesh_path);

	prop->mesh = &mesh_resource->mesh;
	prop->shape = &mesh_resource->shape;
	resource_add_dependency(resource, resource_from_data(mesh_resource));

#if CLIENT
	prop->material = material_load(material_path);
	resource_add_dependency(resource, resource_from_data(prop->material));
#endif
}

void prop_res_free(Resource* resource, Prop_Resource* prop)
{
}

Prop_Resource* prop_resource_load(const char* path)
{
	return resource_load_t(Prop_Resource, path, prop_res_create, prop_res_free);
}