#pragma once
#include "Core/Math/Transform.h"
#include "Engine/Collision/CollisionTypes.h"
struct Convex_Shape;
struct Drawable;

struct Prop
{
	const char* resource_path = nullptr;
	Transform transform;
	Convex_Shape shape;

#if CLIENT
	Drawable* drawable = nullptr;
#endif
};

void prop_init(Prop* prop, const char* path);
void prop_free(Prop* prop);
void prop_set_transform(Prop* prop, const Transform& transform);