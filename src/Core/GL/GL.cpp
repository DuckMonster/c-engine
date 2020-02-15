#define IMPLEMENT_GL
#include "GL.h"

#define IMPORT_FUN(name) name = (fn_ ## name)wglGetProcAddress(#name);\
assert(name != nullptr)

// Load all of those suckers!
void load_opengl_functions()
{
	// Vertex buffers etc.
	IMPORT_FUN(glGenVertexArrays);
	IMPORT_FUN(glDeleteVertexArrays);
	IMPORT_FUN(glBindVertexArray);

	IMPORT_FUN(glGenBuffers);
	IMPORT_FUN(glDeleteBuffers);
	IMPORT_FUN(glBufferData);
	IMPORT_FUN(glBufferSubData);
	IMPORT_FUN(glBindBuffer);

	IMPORT_FUN(glGetAttribLocation);
	IMPORT_FUN(glEnableVertexAttribArray);
	IMPORT_FUN(glVertexAttribPointer);

	// Textures
	IMPORT_FUN(glActiveTexture);

	// Shaders
	IMPORT_FUN(glCreateShader);
	IMPORT_FUN(glDeleteShader);
	IMPORT_FUN(glShaderSource);
	IMPORT_FUN(glGetShaderSource);
	IMPORT_FUN(glCompileShader);
	IMPORT_FUN(glAttachShader);
	IMPORT_FUN(glDetachShader);
	IMPORT_FUN(glGetShaderiv);
	IMPORT_FUN(glGetShaderInfoLog);
	IMPORT_FUN(glCreateProgram);
	IMPORT_FUN(glDeleteProgram);
	IMPORT_FUN(glLinkProgram);
	IMPORT_FUN(glUseProgram);
	IMPORT_FUN(glGetProgramiv);
	IMPORT_FUN(glGetProgramInfoLog);

	// Uniforms
	IMPORT_FUN(glGetUniformLocation);
	IMPORT_FUN(glUniform1i);
	IMPORT_FUN(glUniform1f);
	IMPORT_FUN(glUniform2fv);
	IMPORT_FUN(glUniform3fv);
	IMPORT_FUN(glUniform4fv);
	IMPORT_FUN(glUniformMatrix4fv);

	// Framebuffers
	IMPORT_FUN(glCreateFramebuffers);
	IMPORT_FUN(glBindFramebuffer);
	IMPORT_FUN(glDeleteFramebuffers);
	IMPORT_FUN(glFramebufferTexture2D);
	IMPORT_FUN(glCheckFramebufferStatus);

	// WGL stuff
	IMPORT_FUN(wglChoosePixelFormatARB);
	IMPORT_FUN(wglCreateContextAttribsARB);
	IMPORT_FUN(wglSwapIntervalEXT);
}

// Open GL extensions for 3.3 goodness!
void init_opengl_extensions()
{
	static bool was_extension_loaded = false;
	if (was_extension_loaded)
		return;

	// We create a dummy window, that we can use to initialize GL extensions
	// Because we need it later DURING context creation. So we have to do it twice.
	WNDCLASS dummy_class = { 0 };
	dummy_class.style = CS_OWNDC;
	dummy_class.lpfnWndProc = DefWindowProc;
	dummy_class.hInstance = GetModuleHandle(NULL);
	dummy_class.lpszClassName = "Dummy_OpenGL";

	assert(RegisterClass(&dummy_class));

	HWND dummy_window = CreateWindowEx(
		0,
		dummy_class.lpszClassName,
		"Dummy OpenGL Window",
		0,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		0, 0,
		dummy_class.hInstance,
		0
	);

	// Okay, now that its open lets init all the contexts and stuff
	HDC dummy_dc = GetDC(dummy_window);

	// Setup pixel format
	PIXELFORMATDESCRIPTOR pfd = { 0 };
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.dwFlags = PFD_SUPPORT_OPENGL;

	int pixel_format = ChoosePixelFormat(dummy_dc, &pfd);
	assert(pixel_format);
	assert(SetPixelFormat(dummy_dc, pixel_format, &pfd));

	// Setup gl context
	HGLRC dummy_gl_context = wglCreateContext(dummy_dc);
	assert(wglMakeCurrent(dummy_dc, dummy_gl_context));

	// Init OpenGL extensions!
	load_opengl_functions();

	// Okay, opengl is setup, dummy window has fulfilled its purpose
	wglMakeCurrent(dummy_dc, 0);
	wglDeleteContext(dummy_gl_context);
	ReleaseDC(dummy_window, dummy_dc);
	DestroyWindow(dummy_window);

	was_extension_loaded = true;
}