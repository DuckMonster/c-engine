#include "Material.h"
#include "Core/OS/File.h"
#include "Core/Import/Dat.h"
#include "Engine/Resource/Resource.h"
#include "Engine/Resource/HotReload.h"
#include "Engine/Graphics/Texture.h"
#include <cstdlib>

/* SHADER */
void shader_res_create(Resource* resource, GLenum type)
{
	char* source;
	i32 source_length = file_read_all_dynamic(resource->path, source);
	if (source_length == -1)
	{
		error("Failed to load shader '%s', file not found", resource->path);
	}

	defer { free(source); };

	Shader* shader = new Shader();
	resource->ptr = shader;

	shader->handle = glCreateShader(type);
	glShaderSource(shader->handle, 1, &source, &source_length);
	glCompileShader(shader->handle);
}

void shader_res_create_vert(Resource* resource) { shader_res_create(resource, GL_VERTEX_SHADER); }
void shader_res_create_geom(Resource* resource) { shader_res_create(resource, GL_GEOMETRY_SHADER); }
void shader_res_create_frag(Resource* resource) { shader_res_create(resource, GL_FRAGMENT_SHADER); }

void shader_res_destroy(Resource* resource)
{
	Shader* shader = (Shader*)resource->ptr;

	glDeleteShader(shader->handle);
	delete shader;

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

/* MATERIAL */
void material_res_create(Resource* resource)
{
	Material* material = (Material*)resource->ptr;
	if (material == nullptr)
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
	glAttachShader(material->program, vertex_shdr->handle);
	glAttachShader(material->program, fragment_shdr->handle);

	// Read and load the optional geometry path
	const char* geom_path;
	if (dat_read(doc.root, "geometry", &geom_path))
	{
		const Shader* geometry_shdr = shader_load(GL_GEOMETRY_SHADER, geom_path);
		resource_add_dependency(resource, geom_path);
		glAttachShader(material->program, geometry_shdr->handle);
	}

	glLinkProgram(material->program);

	glDetachShader(material->program, vertex_shdr->handle);
	glDetachShader(material->program, fragment_shdr->handle);

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
			msg_box("Linking of program failed, but there was no info log...");
		}
		else
		{
			char* buffer = (char*)malloc(buffer_len);
			defer { free(buffer); };

			// Get info log
			glGetProgramInfoLog(material->program, buffer_len, nullptr, buffer);
			msg_box("Material error:\n%s", buffer);
		}
	}

	// Read if the material uses a texture
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

	material->texture = nullptr;
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