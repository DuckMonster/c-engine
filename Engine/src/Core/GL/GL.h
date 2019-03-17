#pragma once
#define WIN_LEAN_AND_MEAN
#include <windows.h>
#undef WIN_LEAN_AND_MEAN
#include <GL/GL.h>
#include <GL/wglext.h>
#include <GL/glext.h>

#ifdef IMPLEMENT_GL

// Define this macro if the .cpp is IMPLEMENTING GL-functions (GL_Bind.cpp)
#define DECL_FUN(fn_ptr, name) typedef fn_ptr fn_ ## name;\
fn_ ## name name

#else

// Otherwise, extern all the functions
#define DECL_FUN(fn_ptr, name) typedef fn_ptr fn_ ## name;\
extern fn_ ## name name

#endif

/******* GL functions ******/
// Vertex-buffers and stuff
DECL_FUN(PFNGLGENVERTEXARRAYSPROC,	glGenVertexArrays);
DECL_FUN(PFNGLDELETEVERTEXARRAYSPROC,	glDeleteVertexArrays);
DECL_FUN(PFNGLBINDVERTEXARRAYPROC,	glBindVertexArray);
DECL_FUN(PFNGLGENBUFFERSPROC,		glGenBuffers);
DECL_FUN(PFNGLDELETEBUFFERSPROC,	glDeleteBuffers);
DECL_FUN(PFNGLBUFFERDATAPROC,		glBufferData);
DECL_FUN(PFNGLBINDBUFFERPROC,		glBindBuffer);

DECL_FUN(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray);
DECL_FUN(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer);

// Shaders and programs
DECL_FUN(PFNGLCREATESHADERPROC,		glCreateShader);
DECL_FUN(PFNGLSHADERSOURCEPROC,		glShaderSource);
DECL_FUN(PFNGLCOMPILESHADERPROC,	glCompileShader);
DECL_FUN(PFNGLATTACHSHADERPROC,		glAttachShader);
DECL_FUN(PFNGLCREATEPROGRAMPROC,	glCreateProgram);
DECL_FUN(PFNGLLINKPROGRAMPROC,		glLinkProgram);
DECL_FUN(PFNGLUSEPROGRAMPROC,		glUseProgram);
DECL_FUN(PFNGLGETPROGRAMIVPROC,		glGetProgramiv);
DECL_FUN(PFNGLGETPROGRAMINFOLOGPROC,glGetProgramInfoLog);

// Uniforms!
DECL_FUN(PFNGLGETUNIFORMLOCATIONPROC,	glGetUniformLocation);
DECL_FUN(PFNGLUNIFORM1FPROC,			glUniform1f);
DECL_FUN(PFNGLUNIFORMMATRIX4FVPROC,		glUniformMatrix4fv);

/******* WGL Extension Functions *******/
DECL_FUN(PFNWGLCHOOSEPIXELFORMATARBPROC,	wglChoosePixelFormatARB);
DECL_FUN(PFNWGLCREATECONTEXTATTRIBSARBPROC, wglCreateContextAttribsARB);
DECL_FUN(PFNWGLSWAPINTERVALEXTPROC,			wglSwapIntervalEXT);

// Call this to initialize opengl extensions!
void init_opengl_extensions();