#pragma once
struct Mesh;
struct Material;
struct Texture;
struct Render_State;

struct Drawable
{
	const Mesh* mesh = nullptr;
	const Material* material = nullptr;
	const Texture* texture = nullptr;
	Mat4 transform;
};

void drawable_init();

Drawable* drawable_make(const Mesh* mesh, const Material* material, const Texture* texture = nullptr);
Drawable* drawable_load(const char* mesh_path, const char* material_path, const char* texture_path = nullptr);
void drawable_destroy(Drawable* drawable);

void drawable_render(const Render_State& state);