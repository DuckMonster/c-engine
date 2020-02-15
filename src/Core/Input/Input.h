#pragma once
#include "Key.h"
#include "Mouse.h"

struct Action_State
{
	bool pressed = false;
	u32 frame_num = 0;
};

struct Mouse_State
{
	int x;
	int y;
	int delta_x;
	int delta_y;
	int scroll_delta;

	Action_State buttons[(u32)Mouse_Btn::MAX];
};

struct Input
{
	u32 frame_num;
	Action_State keyboard[(u32)Key::MAX];
	Mouse_State mouse;
};

extern Input input;

// Returns if a key is currently pressed
bool input_key_down(Key key);

// Returns if a key was pressed this frame
bool input_key_pressed(Key key);

// Returns if a key was released this frame
bool input_key_released(Key key);

void input_key_consume(Key key);

// Returns the coordinate of the mouse in pixel-space
int input_mouse_x();
int input_mouse_y();

// Returns the number of pixels moved by the mouse since last frame
int input_mouse_delta_x();
int input_mouse_delta_y();

// Returns if a mouse button is down, pressed this frame or released this frame
bool input_mouse_button_down(Mouse_Btn btn);
bool input_mouse_button_pressed(Mouse_Btn btn);
bool input_mouse_button_released(Mouse_Btn btn);

void input_mouse_button_consume(Mouse_Btn btn);

// Purge all input, resetting the state completely
void input_purge();