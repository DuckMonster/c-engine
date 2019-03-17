#include "Material.h"
#include "Core/OS/File.h"
#include <cstdlib>

void shader_load(GLuint* shader, GLenum type, const char* path)
{
	char* source;
	i32 source_length = file_read_all_dynamic(path, source);
	if (source_length == -1)
	{
		error("Failed to load shader '%s', file not found", path);
		return;
	}

	defer { free(source); };

	*shader = glCreateShader(type);
	glShaderSource(*shader, 1, &source, &source_length);
	glCompileShader(*shader);

	debug_log("Loaded shader '%s'", path);
}

bool material_load(Material* material, const char* vertex_path, const char* fragment_path)
{
	shader_load(&material->vertex, GL_VERTEX_SHADER, vertex_path);
	shader_load(&material->fragment, GL_FRAGMENT_SHADER, fragment_path);

	material->program = glCreateProgram();
	glAttachShader(material->program, material->vertex);
	glAttachShader(material->program, material->fragment);
	glLinkProgram(material->program);

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
			error("Linking of program failed, but there was no info log...");
		}
		else
		{
			char* buffer = (char*)malloc(buffer_len);
			defer { free(buffer); };

			// Get info log
			glGetProgramInfoLog(material->program, buffer_len, nullptr, buffer);
			error(buffer);

			return false;
		}
	}

	return true;
}

bool material_load_standard(Material_Standard* material, const char* vertex_path, const char* fragment_path)
{
	// Load material from source
	bool result = material_load((Material*)material, vertex_path, fragment_path);

	if (!result)
		return false;

	// Load uniforms
	glUseProgram(material->program);
	material->u_viewprojection = glGetUniformLocation(material->program, "u_ViewProjection");
	material->u_time = glGetUniformLocation(material->program, "u_Time");
	material->u_model = glGetUniformLocation(material->program, "u_Model");

	return true;
}