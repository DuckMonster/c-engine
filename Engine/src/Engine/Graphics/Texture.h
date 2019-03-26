#pragma once
#include "Core/GL/GL.h"

struct Texture
{
	GLuint handle;
};

void texture_create(Texture* tex);
void texture_free(Texture* tex);