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
	size_t count;
} d_mesh;

typedef struct {
	vec3 pos;
	vec3 normal;
	vec2 uv;
	color color;
} d_vertex;

d_mesh d_make_mesh(d_vertex*, size_t, unsigned int*, size_t);
d_tex2d d_make_tex(unsigned char*, int, int);
d_program d_make_program(const char*, const char*);

void d_draw(d_mesh*, d_program*);

void d_send_f(const char*, float);
void d_send_vec2(const char*, vec2);
void d_send_vec3(const char*, vec3);
void d_send_color(const char*, color);
void d_send_mat4(const char*, mat4);

void d_push_t();
void d_pop_t();
void d_translate(vec3);
void d_scale(vec3);
void d_rot_x(float);
void d_rot_y(float);
void d_rot_z(float);

#endif

