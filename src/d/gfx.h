// wengwengweng

#ifndef D_GFX_H
#define D_GFX_H

#include "math.h"

static const char* d_vert_default =
"vec4 vert() {"
	"return default_pos();"
"}"
;

static const char* d_frag_default =
"vec4 frag() {"
	"return default_color();"
"}"
;

typedef struct {
	GLuint id;
} d_program;

typedef struct {
	GLuint id;
} d_tex2d;

typedef struct {
	GLuint vbuf;
	GLuint ibuf;
} d_mesh;

typedef struct {
	vec3 pos;
	vec3 normal;
	vec2 uv;
	color color;
} d_vertex;

d_mesh d_make_mesh(float*, size_t, unsigned int*, size_t);
d_program d_make_program(const char*, const char*);
void d_draw(d_mesh*, d_program*);

void d_push_t();
void d_pop_t();

#endif

