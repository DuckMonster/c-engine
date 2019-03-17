#include "Render.h"
#include <cstring>
#include "Core/Time/Time.h"

Render_Queue render_queue;

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
	render_push(Material, material);
}

void render_begin_material_standard(const Material_Standard& material)
{
	render_push_type(Render_Entry_Type::Material_Standard);
	render_push(Material_Standard, material);
}

// UNIFORM
void render_uniform(GLuint location, float value)
{
	Render_Uniform_1F uniform;
	uniform.location = location;
	uniform.value = value;

	render_push_type(Render_Entry_Type::Uniform_1F);
	render_push(Render_Uniform_1F, uniform);
}

void render_uniform(GLuint location, const Mat4& value)
{
	Render_Uniform_Mat uniform;
	uniform.location = location;
	uniform.value = value;

	render_push_type(Render_Entry_Type::Uniform_Mat);
	render_push(Render_Uniform_Mat, uniform);
}

// SET VP
void render_set_vp(const Mat4& vp)
{
	render_push_type(Render_Entry_Type::Set_VP);
	render_push(Mat4, vp);
}

// DRAW MESH
void render_draw_mesh(const Mesh& mesh)
{
	render_push_type(Render_Entry_Type::Draw_Mesh);
	render_push(Mesh, mesh);
}

void render_draw()
{
	u32 size = render_queue.pointer;
	render_queue.pointer = 0;

	// State variables
	Material_Standard* current_material = nullptr;
	Mat4* current_vp = nullptr;
	float time = time_duration();

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

	assert(render_queue.pointer == size);
}

void render_reset()
{
	render_queue.pointer = 0;
}