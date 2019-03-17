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

int input_mouse_x()
{
	return input.mouse.x;
}

int input_mouse_y()
{
	return input.mouse.y;
}