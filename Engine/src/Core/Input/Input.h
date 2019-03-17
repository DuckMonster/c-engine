#pragma once
#include "Key.h"

struct Mouse_State
{
	int x;
	int y;
};

struct Key_State
{
	bool pressed = false;
	u32 frame_num = 0;
};

struct Input
{
	u32 frame_num;
	Key_State keyboard[(u32)Key::MAX];
	Mouse_State mouse;
};

extern Input input;

// Returns if a key is currently pressed
bool input_key_down(Key key);

// Returns if a key was pressed this frame
bool input_key_pressed(Key key);

// Returns if a key was released this frame
bool input_key_released(Key key);

// Returns the x coordinate of the mouse in pixel-space
int input_mouse_x();

// Returns the y coordinate of the mouse in pixel-space
int input_mouse_y();