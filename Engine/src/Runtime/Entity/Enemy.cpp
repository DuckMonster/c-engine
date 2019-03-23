#include "Enemy.h"
#include "Runtime/Scene/Scene.h"
#include "Engine/Graphics/Mesh.h"
#include "Engine/Graphics/Material.h"
#include "Engine/Graphics/Render.h"

void enemies_init()
{
	for(u32 i=0; i<MAX_ENEMIES; ++i)
	{
		scene.enemies[i].is_active = false;
	}
}

void enemy_spawn(const Vec3& position)
{
	for(u32 i=0; i<MAX_ENEMIES; ++i)
	{
		Entity_Enemy& enemy = scene.enemies[i];
		if (enemy.is_active)
			continue;

		enemy.is_active = true;
		enemy.position = position;
		break;
	}
}

void enemies_update()
{
	Entity_Player& player = scene.player;

	for(u32 i=0; i<MAX_ENEMIES; ++i)
	{
		Entity_Enemy& enemy = scene.enemies[i];

		if (!enemy.is_active)
			continue;

		Vec3 direction = normalize(player.position - enemy.position);
		enemy.position += direction * 3.f * time_delta();
	}
}

void enemies_render()
{
	static Mesh mesh;
	static Material_Standard material;
	static bool is_loaded = false;

	if (!is_loaded)
	{
		mesh_load_file(&mesh, "res/Mesh/player.fbx");
		material_load_standard(&material, "res/default.vert", "res/default.frag");
		is_loaded = true;
	}

	render_begin_material_standard(material);
	Mat4 mat;

	for(u32 i=0; i<MAX_ENEMIES; ++i)
	{
		Entity_Enemy& enemy = scene.enemies[i];

		if (!enemy.is_active)
			continue;

		debug_log("Drawing enemy at (%f, %f, %f)", enemy.position.x, enemy.position.y, enemy.position.z);

		mat[3] = Vec4(enemy.position, 1.f);
		render_uniform(material.u_model, mat);
		render_draw_mesh(mesh);
	}
}