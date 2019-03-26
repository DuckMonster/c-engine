#include "Render.h"
#include <cstring>
#include "Core/Time/Time.h"
#include "Core/Context/Context.h"
#include "FrameBuffer.h"

Render_Queue render_queue;

Frame_Buffer get_render_framebuffer()
{
	static Frame_Buffer buffer;
	static bool loaded = false;

	if (!loaded)
	{
		int width = context.width / 4;
		int height = context.height / 4;

		framebuffer_create(&buffer, width, height);
		framebuffer_add_color_texture(&buffer);
		framebuffer_add_depth_texture(&buffer);

		assert(framebuffer_is_complete(&buffer));

		loaded = true;
	}

	return buffer;
}

GLuint get_immediate_vao()
{
	static bool loaded = false;
	static GLuint vao;

	if (!loaded)
	{
		glGenVertexArrays(1, &vao);
		loaded = true;
	}

	return vao;
}

GLuint get_immediate_vbo(u32 index)
{
	static bool loaded = false;
	static GLuint vbo[4];

	assert(index < 4);

	if (!loaded)
	{
		GLuint vao = get_immediate_vao();
		glGenBuffers(4, vbo);

		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, false, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, false, 0, 0);

		glBindVertexArray(0);
		loaded = true;
	}

	return vbo[index];
}

/** FUNCTIONS FOR DATA MANAGEMENT **/
// Allocates (size) bytes in the render-queue and returns a pointer to the data
void* _render_alloc(u32 size)
{
	void* ptr = (render_queue.data + render_queue.pointer);
	render_queue.pointer += size;

	return ptr;
}

void _render_push(const void* ptr, u32 size)
{
	void* data = _render_alloc(size);
	memcpy(data, ptr, size);
}

void render_push_type(Render_Entry_Type type)
{
	Render_Entry_Type* type_ptr = render_alloc(Render_Entry_Type);
	*type_ptr = type;
}

// MATERIAL GROUP
void render_begin_material(const Material& material)
{
	render_push_type(Render_Entry_Type::Material);
	render_push(material);
}

void render_begin_material_standard(const Material_Standard& material)
{
	render_push_type(Render_Entry_Type::Material_Standard);
	render_push(material);
}

// UNIFORM
void render_uniform(GLuint location, float value)
{
	Render_Uniform_1F uniform;
	uniform.location = location;
	uniform.value = value;

	render_push_type(Render_Entry_Type::Uniform_1F);
	render_push(uniform);
}

void render_uniform(GLuint location, const Mat4& value)
{
	Render_Uniform_Mat uniform;
	uniform.location = location;
	uniform.value = value;

	render_push_type(Render_Entry_Type::Uniform_Mat);
	render_push(uniform);
}

// SET VP
void render_set_vp(const Mat4& vp)
{
	render_push_type(Render_Entry_Type::Set_VP);
	render_push(vp);
}

// DRAW MESH
void render_draw_mesh(const Mesh& mesh)
{
	render_push_type(Render_Entry_Type::Draw_Mesh);
	render_push(mesh);
}

void render_draw_immediate(u32 num_verts, float* positions, float* uvs, float* normals)
{
	render_push_type(Render_Entry_Type::Draw_Immediate);

	Render_Immediate* command = render_alloc(Render_Immediate);
	command->vert_count = num_verts;
	command->uvs = nullptr;
	command->normals = nullptr;

	command->positions = (float*)_render_alloc(num_verts * sizeof(Vec3));
	memcpy(command->positions, positions, num_verts * sizeof(Vec3));

	if (uvs != nullptr)
	{
		command->uvs = (float*)_render_alloc(num_verts * sizeof(Vec2));
		memcpy(command->uvs, uvs, num_verts * sizeof(Vec2));
	}
	if (normals != nullptr)
	{
		command->normals = (float*)_render_alloc(num_verts * sizeof(Vec3));
		memcpy(command->normals, normals, num_verts * sizeof(Vec3));
	}
}

void render_flush()
{
	u32 size = render_queue.pointer;
	render_queue.pointer = 0;

	// State variables
	Material_Standard* current_material = nullptr;
	Mat4* current_vp = nullptr;
	float time = time_duration();

	Frame_Buffer framebuffer = get_render_framebuffer();
	framebuffer_bind(&framebuffer);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	while(render_queue.pointer < size)
	{
		Render_Entry_Type* type = render_alloc(Render_Entry_Type);
		switch(*type)
		{
			/** MATERIAL STANDARD **/
			case Render_Entry_Type::Material_Standard:
			{
				current_material = render_alloc(Material_Standard);

				// Use it
				glUseProgram(current_material->program);

				// Set view-projection
				if (current_vp != nullptr)
				{
					glUniformMatrix4fv(current_material->u_viewprojection, 1, false, (float*)current_vp);
				}
				glUniform1f(current_material->u_time, time);
				break;
			}

			/** DRAW MESH **/
			case Render_Entry_Type::Draw_Mesh:
			{
				Mesh* mesh = render_alloc(Mesh);
				glBindVertexArray(mesh->vao);
				if (mesh->use_elements)
				{
					glDrawElements(GL_TRIANGLES, mesh->draw_count, GL_UNSIGNED_INT, 0);
				}
				else
				{
					glDrawArrays(GL_TRIANGLES, 0, mesh->draw_count);
				}
				break;
			}

			/** DRAW IMMEDIATE **/
			case Render_Entry_Type::Draw_Immediate:
			{
				glBindVertexArray(get_immediate_vao());

				Render_Immediate* immediate = render_alloc(Render_Immediate);
				if (immediate->positions)
				{
					glBindBuffer(GL_ARRAY_BUFFER, get_immediate_vbo(0));
					glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3) * immediate->vert_count, immediate->positions, GL_STATIC_DRAW);
				}
				if (immediate->normals)
				{
					glBindBuffer(GL_ARRAY_BUFFER, get_immediate_vbo(1));
					glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3) * immediate->vert_count, immediate->normals, GL_STATIC_DRAW);
				}
				if (immediate->uvs)
				{
					glBindBuffer(GL_ARRAY_BUFFER, get_immediate_vbo(2));
					glBufferData(GL_ARRAY_BUFFER, sizeof(Vec2) * immediate->vert_count, immediate->uvs, GL_STATIC_DRAW);
				}

				glDrawArrays(GL_TRIANGLES, 0, immediate->vert_count);
				glBindVertexArray(0);
				break;
			}

			/** UNIFORMS **/
			case Render_Entry_Type::Uniform_1F:
			{
				Render_Uniform_1F* uniform = render_alloc(Render_Uniform_1F);
				glUniform1f(uniform->location, uniform->value);
				break;
			}
			case Render_Entry_Type::Uniform_Mat:
			{
				Render_Uniform_Mat* uniform = render_alloc(Render_Uniform_Mat);
				glUniformMatrix4fv(uniform->location, 1, false, (float*)&uniform->value);
				break;
			}

			/** SETTING GLOBAL VARIABLES **/
			case Render_Entry_Type::Set_VP:
			{
				current_vp = render_alloc(Mat4);

				// Set view-projection on the current material if there is one
				if (current_material != nullptr)
				{
					glUniformMatrix4fv(current_material->u_viewprojection, 1, false, (float*)current_vp);
				}
				break;
			}
		}
	}

	framebuffer_reset();

	glClearColor(0.1f, 0.f, 0.f, 1.f);
	glDisable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	// TEMP VAO
	float quad_verts[] = {
		-1.f, -1.f, 0.f,
		1.f, -1.f, 0.f,
		1.f, 1.f, 0.f,
		-1.f, 1.f, 0.f
	};
	float quad_uvs[] = {
		0.f, 0.f,
		1.f, 0.f,
		1.f, 1.f,
		0.f, 1.f
	};

	glBindVertexArray(get_immediate_vao());

	glBindBuffer(GL_ARRAY_BUFFER, get_immediate_vbo(0));
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_verts), quad_verts, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, get_immediate_vbo(1));
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_uvs), quad_uvs, GL_STATIC_DRAW);

	// Draw the framebuffer texture to screen
	static Material full_quad_material;
	static bool full_quad_material_loaded = false;

	if (!full_quad_material_loaded)
	{
		material_load(&full_quad_material, "res/full_quad.vert", "res/full_quad.frag");
		full_quad_material_loaded = true;
	}

	glBindTexture(GL_TEXTURE_2D, framebuffer.textures[0].handle);
	glUseProgram(full_quad_material.program);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glBindVertexArray(0);

	assert(render_queue.pointer == size);
}

void render_reset()
{
	render_queue.pointer = 0;
}