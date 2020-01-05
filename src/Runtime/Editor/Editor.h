#pragma once
#include "EdCamera.h"

struct Editor
{
	Ed_Camera camera;
};

#if CLIENT
void editor_init(Editor* editor);
void editor_update(Editor* editor);
void editor_render(Editor* editor);
#endif