#include "Input.h"
Input input;

bool input_key_down(Key key)
{
	return input.keyboard[(u32)key].pressed;
}

bool input_key_pressed(Key key)
{
	return
		input.keyboard[(u32)key].pressed &&
		input.keyboard[(u32)key].frame_num == input.frame_num;
}

bool input_key_released(Key key)
{
	return
		!input.keyboard[(u32)key].pressed &&
		input.keyboard[(u32)key].frame_num == input.frame_num;
}

void input_key_consume(Key key)
{
	input.keyboard[(u32)key].frame_num = 0;
}

int input_mouse_x()
{
	return input.mouse.x;
}

int input_mouse_y()
{
	return input.mouse.y;
}

int input_mouse_delta_x()
{
	return input.mouse.delta_x;
}
int input_mouse_delta_y()
{
	return input.mouse.delta_y;
}

bool input_mouse_button_down(Mouse_Btn btn)
{
	return input.mouse.buttons[(u32)btn].pressed;
}

bool input_mouse_button_pressed(Mouse_Btn btn)
{
	return
		input.mouse.buttons[(u32)btn].pressed &&
		input.mouse.buttons[(u32)btn].frame_num == input.frame_num;
}

bool input_mouse_button_released(Mouse_Btn btn)
{
	return
		!input.mouse.buttons[(u32)btn].pressed &&
		input.mouse.buttons[(u32)btn].frame_num == input.frame_num;
}

void input_mouse_button_consume(Mouse_Btn btn)
{
	input.mouse.buttons[(u32)btn].frame_num = 0;
}

void input_purge()
{
	for(u32 i=0; i<(u32)Mouse_Btn::MAX; ++i)
		input.mouse.buttons[i].pressed = false;
	for(u32 i=0; i<(u32)Key::MAX; ++i)
		input.keyboard[i].pressed = false;
}