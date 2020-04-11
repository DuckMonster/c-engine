#include "FxResource.h"
#include "Engine/Resource/Resource.h"

void fx_res_create(Resource* res, Fx_Resource* fx)
{
	dat_load_file(&fx->doc, res->path);
}

void fx_res_destroy(Resource* res, Fx_Resource* fx)
{
	dat_free(&fx->doc);
}

const Fx_Resource* fx_resource_load(const char* path)
{
	return resource_load_t(Fx_Resource, path, fx_res_create, fx_res_destroy);
}