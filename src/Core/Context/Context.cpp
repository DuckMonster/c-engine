#include "Context.h"
#include <stdio.h>
#include "Core/Windows/WinMin.h"
#include "Core/GL/GL.h"
#include "Core/Input/Input.h"

struct Window_Data
{
	HWND handle;
	HDC context;
	HGLRC gl_context;
};

// Structs for handing events
// WM_KEYDOWN & WM_KEYUP
struct Win_Key_Params
{
	// Repeat count for this key
	u32 repeat		: 16;
	// Hardware scancode of key
	u32 scancode	: 8;
	// Was extended (CTRL/ALT/etc)
	u32 extended	: 1;
	// Dont touch!
	u32 reserved	: 4;
	// Context code, always 0 for WM_KEYDOWN
	u32 context		: 1;
	// Previous key state (1 if key was down, 0 if it was up)
	u32 previous	: 1;
	// Always 0 
	u32 transition	: 1;
};

// WM_MOUSEMOVE
struct Win_MouseMove_Params
{
	u16 x;
	u16 y;
};

// WM_SIZE
struct Win_Size_Params
{
	u16 width;
	u16 height;
};

// Global context
Context context;
Window_Data window;

// WndProc
LRESULT CALLBACK wnd_proc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch(msg)
	{
		// Key down
		case WM_KEYDOWN:
		{
			if (!context.has_focus)
				break;

			Win_Key_Params* key = (Win_Key_Params*)&lparam;

			// Ignore if this is a repeat key
			if (key->previous)
				break;

			// Quit on escape
			if (key->scancode == (u32)Key::Escape)
				SendMessage(wnd, WM_CLOSE, 0, 0);

			Action_State& key_state = input.keyboard[(u32)key->scancode];
			key_state.pressed = true;
			key_state.frame_num = input.frame_num;
			break;
		}

		// Key up
		case WM_KEYUP:
		{
			if (!context.has_focus)
				break;

			Win_Key_Params* key = (Win_Key_Params*)&lparam;

			Action_State& key_state = input.keyboard[(u32)key->scancode];
			key_state.pressed = false;
			key_state.frame_num = input.frame_num;
			break;
		}

		// Mouse move
		case WM_MOUSEMOVE:
		{
			if (!context.has_focus)
				break;

			// Calculate delta first, since we want it even if the cursor is locked
			Win_MouseMove_Params* mouse = (Win_MouseMove_Params*)&lparam;

			// However, don't do that if we dont have focus
			if (context.has_focus)
			{
				input.mouse.delta_x += mouse->x - input.mouse.x;
				input.mouse.delta_y += mouse->y - input.mouse.y;
			}

			if (context.cursor_lock)
			{
				// Convert last mouse position from client to screen
				POINT mouse_point;
				mouse_point.x = input.mouse.x;
				mouse_point.y = input.mouse.y;

				ClientToScreen(wnd, &mouse_point);

				// And lock it there!
				SetCursorPos(mouse_point.x, mouse_point.y);
				break;
			}

			input.mouse.x = mouse->x;
			input.mouse.y = mouse->y;
			break;
		}

		/*** MOUSE BUTTON HELL ***/
		case WM_LBUTTONDOWN:
		{
			Action_State& state = input.mouse.buttons[(u32)Mouse_Btn::Left];
			state.pressed = true;
			state.frame_num = input.frame_num;
			break;
		}
		case WM_LBUTTONUP:
		{
			Action_State& state = input.mouse.buttons[(u32)Mouse_Btn::Left];
			state.pressed = false;
			state.frame_num = input.frame_num;
			break;
		}
		case WM_RBUTTONDOWN:
		{
			Action_State& state = input.mouse.buttons[(u32)Mouse_Btn::Right];
			state.pressed = true;
			state.frame_num = input.frame_num;
			break;
		}
		case WM_RBUTTONUP:
		{
			Action_State& state = input.mouse.buttons[(u32)Mouse_Btn::Right];
			state.pressed = false;
			state.frame_num = input.frame_num;
			break;
		}
		case WM_MBUTTONDOWN:
		{
			Action_State& state = input.mouse.buttons[(u32)Mouse_Btn::Middle];
			state.pressed = true;
			state.frame_num = input.frame_num;
			break;
		}
		case WM_MBUTTONUP:
		{
			Action_State& state = input.mouse.buttons[(u32)Mouse_Btn::Middle];
			state.pressed = false;
			state.frame_num = input.frame_num;
			break;
		}
		case WM_XBUTTONDOWN:
		{
			Action_State& state = input.mouse.buttons[(u32)(wparam == 1 ? Mouse_Btn::X1 : Mouse_Btn::X2)];
			state.pressed = true;
			state.frame_num = input.frame_num;
			break;
		}
		case WM_XBUTTONUP:
		{
			Action_State& state = input.mouse.buttons[(u32)(wparam == 1 ? Mouse_Btn::X1 : Mouse_Btn::X2)];
			state.pressed = false;
			state.frame_num = input.frame_num;
			break;
		}

		// Focus
		case WM_ACTIVATE:
		{
			context.has_focus = wparam > 0;

			if (context.cursor_hide)
				ShowCursor(!context.has_focus);

			if (!context.has_focus)
				context_unlock_cursor();

			// Reset the mouse positions, otherwise we get delta-spikes
			if (context.has_focus)
			{
				POINT mouse_point;
				GetCursorPos(&mouse_point);
				ScreenToClient(window.handle, &mouse_point);

				input.mouse.x = mouse_point.x;
				input.mouse.y = mouse_point.y;
				input.mouse.delta_x = 0;
				input.mouse.delta_y = 0;
			}

			break;
		}

		// Size
		case WM_SIZE:
		{
			Win_Size_Params* size = (Win_Size_Params*)&lparam;

			context.width = size->width;
			context.height = size->height;

			glViewport(0, 0, context.width, context.height);
			break;
		}

		// Closing
		case WM_CLOSE:
		{
			context.is_open = false;
			break;
		}

		// Destroying
		case WM_DESTROY:
		{
			wglDeleteContext(window.gl_context);
			ReleaseDC(window.handle, window.context);

			window.context = NULL;
			window.gl_context = NULL;
			break;
		}
	}

	return DefWindowProc(wnd, msg, wparam, lparam);
}

void context_open(const char* title, u32 x, u32 y, u32 width, u32 height)
{
	// Init opengl!
	init_opengl_extensions();

	// Get module instance
	HINSTANCE instance = GetModuleHandle(NULL);

	// Register window class if we haven't
	static bool class_was_registered = false;
	static LPCSTR class_name = "WindowClass";

	if (!class_was_registered)
	{
		WNDCLASS wc = { 0 };
		wc.lpfnWndProc = wnd_proc;
		wc.hInstance = instance;
		wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
		wc.lpszClassName = class_name;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.style = CS_OWNDC;

		assert(RegisterClass(&wc));
		class_was_registered = true;
	}

	context.x = x;
	context.y = y;
	context.width = width;
	context.height = height;
	context.is_open = true;

	// Open window!
	window.handle = CreateWindow(
		class_name,
		title,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		x, y, width, height,
		0, 0,
		instance,
		0
	);

	/** Create GL context **/
	HDC device_context = GetDC(window.handle);

	// Fetch pixel format
	int pixel_format_attribs[] = {
		WGL_DRAW_TO_WINDOW_ARB,		GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB,		GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB,		GL_TRUE,
		WGL_ACCELERATION_ARB,		WGL_FULL_ACCELERATION_ARB,
		WGL_PIXEL_TYPE_ARB,			WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB,			32,
		WGL_DEPTH_BITS_ARB,			24,
		WGL_STENCIL_BITS_ARB,		8,
		0
	};

	int pixel_format;
	UINT num_formats;

	assert(wglChoosePixelFormatARB(device_context, pixel_format_attribs, 0, 1, &pixel_format, &num_formats));
	assert(num_formats);

	// Set that format
	PIXELFORMATDESCRIPTOR pfd;
	DescribePixelFormat(device_context, pixel_format, sizeof(pfd), &pfd);
	assert(SetPixelFormat(device_context, pixel_format, &pfd));

	// Initialize 3.3 context
	int context_attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB,		3,
		WGL_CONTEXT_MINOR_VERSION_ARB,		3,
		WGL_CONTEXT_PROFILE_MASK_ARB,		WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	HGLRC gl_context = wglCreateContextAttribsARB(device_context, 0, context_attribs);
	assert(gl_context);

	wglMakeCurrent(device_context, gl_context);

	// Disable VSYNC, bro
	wglSwapIntervalEXT(0);

	window.context = device_context;
	window.gl_context = gl_context;
}

void context_close()
{
	// Destroy context if there is one
	if (context.is_open)
	{
		DestroyWindow(window.handle);
		window.handle = nullptr;
	}
}

void context_set_title(const char* title)
{
	SetWindowText(window.handle, title);
}

void context_begin_frame()
{
	input.frame_num++;
	input.mouse.delta_x = 0;
	input.mouse.delta_y = 0;

	// Read events
	MSG msg = { 0 };
	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void context_focus()
{
	SetForegroundWindow(window.handle);
}

void context_end_frame()
{
	SwapBuffers(window.context);
	Sleep(1);
}

void context_hide_cursor()
{
	context.cursor_hide = true;
	ShowCursor(false);
}

void context_show_cursor()
{
	context.cursor_hide = false;

	// Keep incrementing the show counter until its visible
	while(ShowCursor(true) < 0); 
}

void context_lock_cursor()
{
	context.cursor_lock = true;
}

void context_unlock_cursor()
{
	context.cursor_lock = false;
}

void context_set_topmost(bool topmost)
{
	SetWindowPos(
		window.handle, topmost ? HWND_TOPMOST : HWND_NOTOPMOST,
		0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE
	);
}

void* context_get_handle()
{
	return &window.handle;
}