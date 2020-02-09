#pragma once

struct Tga_File
{
	u16 width;
	u16 height;
	u8 channels;
	void* data;
};

bool tga_load(Tga_File* tga, const char* path);
void tga_free(Tga_File* tga);