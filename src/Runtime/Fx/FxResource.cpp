#include "FxResource.h"
#include "Engine/Resource/Resource.h"

void fx_res_create(Resource* res)
{
	Fx_Resource* fx = (Fx_Resource*)res->ptr;
	if (!fx)
	{
		fx = new Fx_Resource();
		res->ptr = fx;
	}

	dat_load_file(&fx->doc, res->path);
}

void fx_res_destroy(Resource* res)
{
	Fx_Resource* fx = (Fx_Resource*)res->ptr;
	dat_free(&fx->doc);
}

const Fx_Resource* fx_resource_load(const char* path)
{
	Resource* res = resource_load(path, fx_res_create, fx_res_destroy);
	return (Fx_Resource*)res->ptr;
}