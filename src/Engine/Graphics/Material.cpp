#include "Material.h"
#include "Core/OS/File.h"
#include "Core/Import/Dat.h"
#include "Engine/Resource/Resource.h"
#include "Engine/Resource/HotReload.h"
#include "Engine/Graphics/Texture.h"
#include <cstdlib>
#include <stdio.h>

const char* SHADER_VERSION_STR = "#version 330 core\n";

/* SHADER */
void shader_res_create(Resource* resource, GLenum type)
{
	Shader* shader = new Shader();
	resource->ptr = shader;

	shader->type = type;
	i32 length = file_read_all_dynamic(resource->path, &shader->source);
	if (length <= 0)
	{
		msg_box("Failed to load shader '%s', file not found", resource->path);
		shader->source = nullptr;
		return;
	}
	else
	{
		shader->source_len = length;
	}

	// Pre-process for include directives
	const char* preprocess_str = shader->source;
	const char* include_str = nullptr;

	while(include_str = strstr(preprocess_str, "#include"))
	{
		const char* include_path = nullptr;
		u32 include_path_len = 0;

		const char* str = include_str;
		while(*str != '\n')
		{
			// Opening/closing quotes
			if (*str == '\"')
			{
				if (include_path == nullptr)
					include_path = str + 1;
				else
					include_path_len = str - include_path;
			}

			debug_log("'%s' includes '%.*s'", resource->path, include_path_len, include_path);
		}

		preprocess_str = include_str + 8;
	}
}

void shader_res_create_vert(Resource* resource) { shader_res_create(resource, GL_VERTEX_SHADER); }
void shader_res_create_geom(Resource* resource) { shader_res_create(resource, GL_GEOMETRY_SHADER); }
void shader_res_create_frag(Resource* resource) { shader_res_create(resource, GL_FRAGMENT_SHADER); }

void shader_res_destroy(Resource* resource)
{
	Shader* shader = (Shader*)resource->ptr;
	if (shader->source)
		free(shader->source);
	free(shader);

	resource->ptr = nullptr;
}

const Shader* shader_load(GLenum type, const char* path)
{
	Resource* resource = nullptr;
	switch(type)
	{
		case GL_VERTEX_SHADER:
			resource = resource_load(path, shader_res_create_vert, shader_res_destroy);
			break;

		case GL_GEOMETRY_SHADER:
			resource = resource_load(path, shader_res_create_geom, shader_res_destroy);
			break;

		case GL_FRAGMENT_SHADER:
			resource = resource_load(path, shader_res_create_frag, shader_res_destroy);
			break;

		default:
			error("'%s' trying to load invalid shader type %d", path, type);
			break;
	}

	return (Shader*)resource->ptr;
}

GLuint shader_compile(const Shader* shader, const char* const* defines, u32 num_defines)
{
	GLuint handle = glCreateShader(shader->type);

	// Total number of sources: version directive -> defines -> source code
	u32 num_sources = 2 + num_defines;

	const char** sources = new const char*[num_sources];
	i32* source_lengths = new i32[num_sources];
	defer { free(sources); free(source_lengths); };

	// First, version directive
	sources[0] = SHADER_VERSION_STR;
	source_lengths[0] = -1;

	// Second, all defines
	for(u32 i=0; i<num_defines; ++i)
	{
		sources[i + 1] = defines[i];
		source_lengths[i + 1] = -1;
	}

	// And last, the source of the shader!
	sources[num_sources - 1] = shader->source;
	source_lengths[num_sources - 1] = shader->source_len;

	glShaderSource(handle, num_sources, sources, source_lengths);
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
			msg_box("Compiling shader failed, but there was no info log...");
		}
		else
		{
			char* buffer = (char*)malloc(buffer_len);
			defer { free(buffer); };

			// Get info log
			glGetShaderInfoLog(handle, buffer_len, nullptr, buffer);
			msg_box("Shader error:\n%s", buffer);
		}
	}

	return handle;
}

/* MATERIAL */
void material_res_create(Resource* resource)
{
	Material* material = (Material*)resource->ptr;
	if (!material)
	{
		material = new Material();
		resource->ptr = material;
	}

	material->program = glCreateProgram();

	// Read material dat file
	Dat_Document doc;
	if (!dat_load_file(&doc, resource->path))
	{
		error("Failed to load material file '%s'", resource->path);
	}

	defer { dat_free(&doc); };

	// Read define-list
	Dat_Object* define_obj = dat_get_object(doc.root, "defines");
	char** defines = nullptr;
	u32 num_defines = 0;

	if (define_obj != nullptr)
	{
		Dat_Key* key = define_obj->first_key;

		// Count how many defines there are...
		while(key)
		{
			if (key->value->type == Dat_Node_Type::ValueRaw)
				num_defines++;

			key = key->next;
		}

		key = define_obj->first_key;

		// Read through and build all of them!
		defines = new char*[num_defines];
		u32 define_index = 0;

		// Loop through all keys with raw values, and add them as defines
		while(key)
		{
			Dat_Node* value = key->value;

			// Only raw values...
			if (value->type == Dat_Node_Type::ValueRaw)
			{
				// Hard-coded 80 bytes, hopefully no defines will be bigger......
				defines[define_index] = new char[80];

				Dat_Value_Raw* value_raw = (Dat_Value_Raw*)value;
				sprintf(defines[define_index], "#define %.*s %.*s\n", 
					key->name_len, key->name,
					value_raw->str_len, value_raw->str
				);

				define_index++;
			}

			key = key->next;
		}

		assert(define_index == num_defines);
	}

	// Read the vertex and fragment file paths
	const char* vert_path;
	const char* frag_path;

	if (!dat_read(doc.root, "vertex", &vert_path))
	{
		msg_box("Failed to load material '%s', vertex shader path not specified", resource->path);
		return;
	}
	if (!dat_read(doc.root, "fragment", &frag_path))
	{
		msg_box("Failed to load material '%s', fragment shader path not specified", resource->path);
		return;
	}

	const Shader* vertex_shdr = shader_load(GL_VERTEX_SHADER, vert_path);
	const Shader* fragment_shdr = shader_load(GL_FRAGMENT_SHADER, frag_path);

	resource_add_dependency(resource, vert_path);
	resource_add_dependency(resource, frag_path);

	material->vertex = shader_compile(vertex_shdr, defines, num_defines);
	material->fragment = shader_compile(fragment_shdr, defines, num_defines);
	glAttachShader(material->program, material->vertex);
	glAttachShader(material->program, material->fragment);

	// Read and load the optional geometry path
	const char* geom_path;
	if (dat_read(doc.root, "geometry", &geom_path))
	{
		const Shader* geometry_shdr = shader_load(GL_GEOMETRY_SHADER, geom_path);
		resource_add_dependency(resource, geom_path);

		material->geometry = shader_compile(geometry_shdr, defines, num_defines);
		glAttachShader(material->program, material->geometry);
	}

	glLinkProgram(material->program);
	glDetachShader(material->program, material->vertex);
	glDetachShader(material->program, material->fragment);

	if (material->geometry != GL_INVALID_INDEX)
		glDetachShader(material->program, material->geometry);

	// Check if everything succeeded
	GLint success = 0;
	glGetProgramiv(material->program, GL_LINK_STATUS, &success);

	if (success != GL_TRUE)
	{
		// Uh oh!
		// Get and allocate space for the info log buffer
		GLint buffer_len = 0;
		glGetProgramiv(material->program, GL_INFO_LOG_LENGTH, &buffer_len);

		if (buffer_len == 0)
		{
			msg_box("Linking of program '%s' failed, but there was no info log...", resource->path);
		}
		else
		{
			char* buffer = (char*)malloc(buffer_len);
			defer { free(buffer); };

			// Get info log
			glGetProgramInfoLog(material->program, buffer_len, nullptr, buffer);
			msg_box("'%s' material error:\n%s", resource->path, buffer);
		}
	}

	if (defines)
	{
		for(u32 i=0; i<num_defines; ++i)
			free(defines[i]);

		free(defines);
	}

	// Read if the material uses a texture
	material->texture = nullptr;

	const char* texture_path;
	if (dat_read(doc.root, "texture", &texture_path))
	{
		const Texture* texture = texture_load(texture_path);
		resource_add_dependency(resource, texture_path);

		material->texture = texture;
	}
}

void material_res_destroy(Resource* resource)
{
	Material* material = (Material*)resource->ptr;
	glDeleteProgram(material->program);

	if (material->vertex != GL_INVALID_INDEX)
		glDeleteShader(material->vertex);
	if (material->fragment != GL_INVALID_INDEX)
		glDeleteShader(material->fragment);
	if (material->geometry != GL_INVALID_INDEX)
		glDeleteShader(material->geometry);

	*material = Material();
}

const Material* material_load(const char* path)
{
	// Load resource
	Resource* resource = resource_load(path, material_res_create, material_res_destroy);
	return (Material*)resource->ptr;
}

void material_bind(const Material* mat)
{
	glUseProgram(mat->program);

	if (mat->texture)
		texture_bind(mat->texture, 0);
}

/* MATERIAL UNIFORM SETTERS */
void material_set(const Material* mat, const char* name, const int value)
{
	GLuint uniform = glGetUniformLocation(mat->program, name);
	glUniform1i(uniform, value);
}

void material_set(const Material* mat, const char* name, const float value)
{
	GLuint uniform = glGetUniformLocation(mat->program, name);
	glUniform1f(uniform, value);
}

void material_set(const Material* mat, const char* name, const Vec2& value)
{
	GLuint uniform = glGetUniformLocation(mat->program, name);
	glUniform2fv(uniform, 1, (float*)&value);
}

void material_set(const Material* mat, const char* name, const Vec3& value)
{
	GLuint uniform = glGetUniformLocation(mat->program, name);
	glUniform3fv(uniform, 1, (float*)&value);
}

void material_set(const Material* mat, const char* name, const Vec4& value)
{
	GLuint uniform = glGetUniformLocation(mat->program, name);
	glUniform4fv(uniform, 1, (float*)&value);
}

void material_set(const Material* mat, const char* name, const Mat4& value)
{
	GLuint uniform = glGetUniformLocation(mat->program, name);
	glUniformMatrix4fv(uniform, 1, false, (float*)&value);
}