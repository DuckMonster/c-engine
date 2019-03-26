#include "Texture.h"

void texture_create(Texture* tex)
{
	glGenTextures(1, &tex->handle);
	glBindTexture(GL_TEXTURE_2D, tex->handle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void texture_free(Texture* tex)
{
	glDeleteTextures(1, &tex->handle);
}