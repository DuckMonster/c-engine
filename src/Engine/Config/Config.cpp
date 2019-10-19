#include "Config.h"
#include "Core/Import/Dat.h"

Dat_Document doc;

void config_load(const char* path)
{
	dat_load_file(&doc, path);
}

bool config_get(const char* name, bool* value)
{
	return dat_read(doc.root, name, value);
}
bool config_get(const char* name, i16* value)
{
	return dat_read(doc.root, name, value);
}
bool config_get(const char* name, u16* value)
{
	return dat_read(doc.root, name, value);
}
bool config_get(const char* name, i32* value)
{
	return dat_read(doc.root, name, value);
}
bool config_get(const char* name, u32* value)
{
	return dat_read(doc.root, name, value);
}

bool config_get(const char* name, const char** value)
{
	return dat_read(doc.root, name, value);
}