#include "FrameBuffer.h"
#include "Core/Context/Context.h"

#define FRAMEBUFFER_STACK_SIZE
namespace
{
	Frame_Buffer* buffer_stack[4];
	u32 buffer_stack_index = 0;
}

void framebuffer_create(Frame_Buffer* fb, u32 width, u32 height)
{
	glCreateFramebuffers(1, &fb->handle);
	fb->width = width;
	fb->height = height;
}

void framebuffer_add_color_texture(Frame_Buffer* fb)
{
	Texture& texture = fb->textures[fb->num_textures++];
	texture_create(&texture);

	glBindTexture(GL_TEXTURE_2D, texture.handle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fb->width, fb->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	glBindFramebuffer(GL_FRAMEBUFFER, fb->handle);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.handle, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void framebuffer_add_depth_texture(Frame_Buffer* fb)
{
	Texture& texture = fb->textures[fb->num_textures++];
	texture_create(&texture);

	glBindTexture(GL_TEXTURE_2D, texture.handle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, fb->width, fb->height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	glBindFramebuffer(GL_FRAMEBUFFER, fb->handle);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture.handle, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool framebuffer_is_complete(Frame_Buffer* fb)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fb->handle);
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return status == GL_FRAMEBUFFER_COMPLETE;
}

void framebuffer_free(Frame_Buffer* fb)
{
	glDeleteFramebuffers(1, &fb->handle);

	for(u32 i=0; i<fb->num_textures; ++i)
		glDeleteTextures(1, &fb->textures[i].handle);
}

void framebuffer_bind(Frame_Buffer* fb)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fb->handle);
	glViewport(0, 0, fb->width, fb->height);
}

void framebuffer_reset()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, context.width, context.height);
}

void framebuffer_push(Frame_Buffer* fb)
{
	buffer_stack[buffer_stack_index] = fb;
	buffer_stack_index++;

	framebuffer_bind(fb);
}

void framebuffer_pop()
{
	if (buffer_stack_index <= 0)
		error("Can't pop framebuffer stack when index is 0");

	buffer_stack_index--;
	if (buffer_stack_index > 0)
		framebuffer_bind(buffer_stack[buffer_stack_index - 1]);
	else
		framebuffer_reset();
}

Frame_Buffer* framebuffer_get_current()
{
	if (buffer_stack_index == 0)
		return nullptr;

	return buffer_stack[buffer_stack_index - 1];
}