#pragma once
#include "Core/Import/Dat.h"

struct Fx_Resource
{
	Dat_Document doc;
};

const Fx_Resource* fx_resource_load(const char* path);