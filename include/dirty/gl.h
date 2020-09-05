// wengwengweng

#ifndef D_GL_H
#define D_GL_H

#include "platform.h"

#define GL_SILENCE_DEPRECATION

#if defined(D_MACOS)
#include <OpenGL/gl.h>
#elif defined(D_IOS)
#include <OpenGLES/ES2/gl.h>
#elif defined(D_LINUX)
#include <GL/gl.h>
#elif defined(D_ANDROID)
#include <GLES2/gl2.h>
#elif defined(D_WINDOWS)
#include <GL/gl.h>
#elif defined(D_WEB)
#include <GLES2/gl2.h>
#endif

#define STR(str) #str
#define GLSL(str) (char*)"#version 120\n" #str

#endif

