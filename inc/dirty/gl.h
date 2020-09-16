// wengwengweng

#ifndef D_GL_H
#define D_GL_H

#include "platform.h"

#define GL_SILENCE_DEPRECATION
#define GLES_SILENCE_DEPRECATION

#if defined(D_MACOS)
#include <OpenGL/gl.h>
#elif defined(D_IOS)
#define GLES
#include <OpenGLES/ES2/gl.h>
#elif defined(D_LINUX)
#include <GL/gl.h>
#elif defined(D_ANDROID)
#define GLES
#include <GLES2/gl2.h>
#elif defined(D_WINDOWS)
#include <GL/gl.h>
#elif defined(D_WEB)
#define GLES
#include <GLES2/gl2.h>
#else
#error platform not supported.
#endif

#endif

