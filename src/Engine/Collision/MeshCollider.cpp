#include "MeshCollider.h"
#include "Engine/Resource/Resource.h"
#include "Core/Import/Fbx.h"

void mesh_collider_res_create(Resource* resource)
{
	Mesh_Collider* collider = (Mesh_Collider*)resource->ptr;
	if (collider == nullptr)
	{
		collider = new Mesh_Collider();
		resource->ptr = collider;
	}

	Fbx_Scene* scene = fbx_import(resource->path);
	if (scene == nullptr)
		return;

	defer { fbx_free(scene); };

	Fbx_Mesh& mesh = scene->meshes[0];
}

void mesh_collider_res_destroy(Resource* resource)
{

}

const Mesh_Collider* mesh_collider_load(const char* path)
{
	Resource* resource =  resource_load(path, mesh_collider_res_create, mesh_collider_res_destroy);
	return (Mesh_Collider*)resource->ptr;
}