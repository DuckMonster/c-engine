#include "Shader.h"
#include "Core/OS/File.h"
#include "Core/OS/Path.h"
#include "Engine/Config/Config.h"
#include "Engine/Resource/Resource.h"
#include "Engine/Resource/HotReload.h"
#include <stdio.h>
const char* SHADER_VERSION_STR = "#version 330 core\n";

i32 parse_include_directive(const char* directive, char* out_path, u32* out_path_len)
{
	*out_path_len = 0;
	const char* new_line = strstr(directive, "\n");
	const char* path_start = nullptr;

	// Get the path outta there
	for(const char* ptr = directive; ptr < new_line; ++ptr)
	{
		if (*ptr == '\"')
		{
			// We've already found the start, so save the length!
			if (path_start)
			{
				u32 path_len = ptr - path_start;

				memcpy(out_path, path_start, path_len);
				out_path[path_len] = 0;
				*out_path_len = path_len;
			}
			else
				path_start = ptr + 1;
		}
	}

	// We want to return the length of the whole directive, so find the newline
	return (new_line - directive);
}

void shader_res_create(Resource* resource, Shader* shader, GLenum type)
{
	shader->path = resource->path;
	shader->type = type;

	i32 length = file_read_all_dynamic(resource->path, &shader->source);
	if (length < 0)
	{
		msg_box("Failed to load shader '%s', file not found", resource->path);
		shader->source = nullptr;
		shader->source_len = 0;
		return;
	}

	shader->source_len = length;

	// Pre-process for include directives
	char* preprocess_str = shader->source;
	char* include_str = nullptr;

	while(include_str = strstr(preprocess_str, "#include"))
	{
		preprocess_str += 10;

		 // Make sure the include directive immediately follows a new-line
		if (include_str != shader->source && *(include_str - 1) != '\n' && *(include_str - 1) != '\r')
			continue;

		// Hopefully not longer than 80.......
		static char path_buffer[80];

		// Parse for include path
		u32 path_len = 0;
		i32 directive_len = parse_include_directive(include_str, path_buffer, &path_len);

		// Replace the whole directive with spaces
		for(char* ptr = include_str; ptr < include_str + directive_len; ++ptr)
			*ptr = ' ';

		if (shader->num_includes >= MAX_INCLUDES)
		{
			msg_box("Shader '%s' failed to include shader '%s', it exceeded the maximum amount of includes", resource->path, path_buffer);
			continue;
		}

		path_buffer[path_len] = 0;

		// Load included shader
		const Shader* include_shader = shader_load(type, path_buffer);
		if (include_shader == nullptr)
		{
			msg_box("Shader '%s' failed to include shader '%s', it failed to load", resource->path, path_buffer);
			continue;
		}

		resource_add_dependency(resource, resource_from_data(include_shader));
		shader->includes[shader->num_includes] = include_shader;
		shader->num_includes++;
	}
}

void shader_res_create_vert(Resource* resource, Shader* shader) { shader_res_create(resource, shader, GL_VERTEX_SHADER); }
void shader_res_create_geom(Resource* resource, Shader* shader) { shader_res_create(resource, shader, GL_GEOMETRY_SHADER); }
void shader_res_create_frag(Resource* resource, Shader* shader) { shader_res_create(resource, shader, GL_FRAGMENT_SHADER); }

void shader_res_destroy(Resource* resource)
{
	Shader* shader = (Shader*)resource->ptr;
	if (shader->source)
		free(shader->source);
}

const Shader* shader_load(GLenum type, const char* path)
{
	switch(type)
	{
		case GL_VERTEX_SHADER:
			return resource_load_t(Shader, path, shader_res_create_vert, shader_res_destroy);

		case GL_GEOMETRY_SHADER:
			return resource_load_t(Shader, path, shader_res_create_geom, shader_res_destroy);

		case GL_FRAGMENT_SHADER:
			return resource_load_t(Shader, path, shader_res_create_frag, shader_res_destroy);

		default:
			error("'%s' trying to load invalid shader type %d", path, type);
			return nullptr;
	}
}

struct Shader_Include_Stack
{
	const Shader* shader;
	Shader_Include_Stack* prev = nullptr;
};
Shader_Include_Stack* include_stack = nullptr;
Shader_Include_Stack* include_list = nullptr;

void shader_include_list_add(const Shader* shader)
{
	Shader_Include_Stack* entry = new Shader_Include_Stack();
	entry->shader = shader;
	entry->prev = include_list;

	include_list = entry;
}
void shader_include_list_empty()
{
	Shader_Include_Stack* entry = include_list;
	while(entry)
	{
		Shader_Include_Stack* prev = entry->prev;
		delete entry;

		entry = prev;
	}

	include_list = nullptr;
}
bool shader_include_list_contains(const Shader* shader)
{
	Shader_Include_Stack* list = include_list;
	while(list)
	{
		if (list->shader == shader)
			return true;

		list = list->prev;
	}

	return false;
}

void shader_include_push(const Shader* shader)
{
	Shader_Include_Stack* entry = new Shader_Include_Stack();
	entry->shader = shader;
	entry->prev = include_stack;

	include_stack = entry;
}

void shader_include_pop()
{
	if (!assert(include_stack != nullptr))
		return;

	Shader_Include_Stack* prev = include_stack->prev;
	delete include_stack;

	include_stack = prev;
}

bool shader_include_stack_contains(const Shader* shader)
{
	Shader_Include_Stack* stack = include_stack;
	while(stack)
	{
		if (stack->shader == shader)
			return true;

		stack = stack->prev;
	}

	return false;
}

void shader_write_includes_recursive(String_Builder* builder, const Shader* shader)
{
	// This shader is already in the include stack! We have a circular dependency...
	if (shader_include_stack_contains(shader))
	{
		String_Builder msg;
		str_append(&msg, "Shader circular dependency\n\n");
		str_append_format(&msg, "%s\n", shader->path);

		Shader_Include_Stack* stack = include_stack;
		while(stack)
		{
			str_append_format(&msg, "%s =>\n", stack->shader->path);
			stack = stack->prev;
		}

		msg_box(msg.str);
		str_free(&msg);
		return;
	}

	// Aready included, ignore...
	if (shader_include_list_contains(shader))
		return;

	shader_include_list_add(shader);
	shader_include_push(shader);

	for(u32 i=0; i<shader->num_includes; ++i)
	{
		shader_write_includes_recursive(builder, shader->includes[i]);
	}

	str_append(builder, shader->source, shader->source_len);
	str_append(builder, "\n");

	shader_include_pop();
}

GLuint shader_compile(const Shader* shader, const char* const* defines, u32 num_defines)
{
	String_Builder source;
	
	// Write version
	str_append(&source, SHADER_VERSION_STR);

	// Write defines
	for(u32 i=0; i<num_defines; ++i)
	{
		str_append(&source, defines[i]);
		str_append(&source, "\n");
	}

	// Write all includes!
	shader_write_includes_recursive(&source, shader);
	shader_include_list_empty();

	GLuint handle = glCreateShader(shader->type);

	// Should we write full source?
	bool should_dump_shaders;
	config_get("render.dump_shaders", &should_dump_shaders);
	if (should_dump_shaders)
	{
		char dump_path[128];
		char* file_name = path_get_file(shader->path);
		defer { free(file_name); };

		sprintf(dump_path, "../build/ShaderDump/%s", file_name);
		file_create_write(dump_path, source.str, source.length);
	}

	glShaderSource(handle, 1, &source.str, nullptr);
	glCompileShader(handle);

	// Check if everything succeeded
	GLint success = 0;
	glGetShaderiv(handle, GL_COMPILE_STATUS, &success);

	if (success != GL_TRUE)
	{
		// Uh oh!
		// Get and allocate space for the info log buffer
		GLint buffer_len = 0;
		glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &buffer_len);

		if (buffer_len == 0)
		{
			debug_log("Compiling shader '%s' failed, but there was no info log...", shader->path);
		}
		else
		{
			char* buffer = (char*)malloc(buffer_len);
			defer { free(buffer); };

			// Get info log
			glGetShaderInfoLog(handle, buffer_len, nullptr, buffer);
			debug_log("Shader '%s' error:\n%s", shader->path, buffer);
		}

		glDeleteShader(handle);
		return GL_INVALID_INDEX;
	}

	str_free(&source);

	return handle;
}
