#include "Editor.h"
#include "Runtime/Render/Render.h"

#if CLIENT

void editor_init(Editor* editor)
{

}

void editor_update(Editor* editor)
{
	ed_camera_update(&editor->camera);
	render_set_vp(ed_camera_view_matrix(&editor->camera), ed_camera_projection_matrix(&editor->camera));
}

void editor_render(Editor* editor)
{

}

#endif