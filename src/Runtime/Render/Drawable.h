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

#if CLIENT

void drawable_init(Drawable* drawable, const Mesh* mesh = nullptr, const Material* material = nullptr, const Texture* texture = nullptr);
void drawable_render(const Drawable* drawable, const Render_State& state);

#endif