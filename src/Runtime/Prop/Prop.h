#pragma once
#include "Core/Math/Transform.h"
struct Drawable;

struct Prop
{
	Transform transform;

#if CLIENT
	Drawable* drawable = nullptr;
#endif
};

void prop_init(Prop* prop, const char* path);
void prop_free(Prop* prop);
void prop_set_transform(Prop* prop, const Transform& transform);