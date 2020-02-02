#include "PropResource.h"
#include "Core/Import/Dat.h"
#include "Engine/Resource/Resource.h"
#include "Engine/Resource/HotReload.h"
#include "Engine/Graphics/MeshResource.h"
#include "Engine/Graphics/Material.h"

void prop_res_create(Resource* resource)
{
	Prop_Resource* prop = (Prop_Resource*)resource->ptr;
	if (!prop)
	{
		prop = new Prop_Resource();
		resource->ptr = prop;
	}

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
	prop->material = material_load(material_path);

	resource_add_dependency(resource, mesh_path);
	resource_add_dependency(resource, material_path);
}

void prop_res_free(Resource* resource)
{
}

Prop_Resource* prop_resource_load(const char* path)
{
	Resource* res = resource_load(path, prop_res_create, prop_res_free);
	return (Prop_Resource*)res->ptr;
}