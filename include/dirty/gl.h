// wengwengweng

#ifndef D_GL_H
#define D_GL_H

#define GL_SILENCE_DEPRECATION

#if defined(__APPLE__)
#include <OpenGL/gl.h>
#elif defined(__EMSCRIPTEN__)
#include <GLES2/gl2.h>
#endif

#define STR(str) #str
#define GLSL(str) (char*)"#version 120\n" #str

#endif

