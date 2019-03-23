#pragma once

#define RENDER_QUEUE_SIZE 1024
#include "Core/GL/GL.h"
#include "Material.h"
#include "Mesh.h"

enum class Render_Entry_Type : u8
{
	/** Materials **/
	Material,
	Material_Standard,

	/** Uniforms **/
	Uniform_1F,
	Uniform_Mat,

	/** Settings global render variables **/
	Set_VP,

	/** Draw mesh **/
	Draw_Mesh,

	/** Draw Immediate **/
	Draw_Immediate,
};

// Command for immediately drawing some vertices
struct Render_Immediate
{
	u32 vert_count;
	float* positions = nullptr;
	float* uvs = nullptr;
	float* normals = nullptr;
};

// Command for setting a float uniform
struct Render_Uniform_1F
{
	GLuint location;
	float value;
};

// Command for setting a matrix uniform
struct Render_Uniform_Mat
{
	GLuint location;
	Mat4 value;
};

// Structure for containing a queue of rendering commands
struct Render_Queue
{
	u8 data[RENDER_QUEUE_SIZE];
	u32 pointer = 0;
};

// Allocates an object onto the render queue and returns a pointer
#define render_alloc(type) (type*)(_render_alloc(sizeof(type)))
void* _render_alloc(u32 size);

// Push some value onto the render queue and advance the pointer
#define render_push(data) _render_push(&data, sizeof(data))
void _render_push(const void* ptr, u32 size);

// Pushes an entry enum onto the render queue
void render_push_type(Render_Entry_Type type);

// Change onto some generic material without standard uniform values
void render_begin_material(const Material& material);
// Change onto a standard material
void render_begin_material_standard(const Material_Standard& material);

// Set a float uniform
void render_uniform(GLuint location, float value);
// Set a materix uniform
void render_uniform(GLuint location, const Mat4& value);

// Set the global ViewProjection matrix for rendering from now on
void render_set_vp(const Mat4& vp);

// Draw a mesh
void render_draw_mesh(const Mesh& mesh);

// For drawing immidiate mode style brother
void render_draw_immediate(u32 num_verts, float* positions, float* uvs, float* normals);

// Advance through a queue and draw to screen
void render_flush();

// Reset a render queue
void render_reset();

extern Render_Queue render_queue;