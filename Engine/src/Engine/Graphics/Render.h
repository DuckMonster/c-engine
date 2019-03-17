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
};

// Structure for containing a queue of rendering commands
struct Render_Queue
{
	u8 data[RENDER_QUEUE_SIZE];
	u32 pointer = 0;
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

// Allocates an object onto the render queue and returns a pointer
#define render_alloc(type) (type*)(_render_alloc(sizeof(type)))
void* _render_alloc(u32 size);

// Push some value onto the render queue and advance the pointer
#define render_push(type, data) _render_push(&data, sizeof(type))
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

// Advance through a queue and draw to screen
void render_draw();

// Reset a render queue
void render_reset();

extern Render_Queue render_queue;