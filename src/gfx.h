// wengwengweng

#ifndef D_GFX_H
#define D_GFX_H

#include "gl.h"
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
	mat4 view;
	mat4 proj;
} d_cam;

typedef struct {
	GLuint id;
} d_program;

typedef struct {
	GLuint id;
} d_tex2d;

typedef struct {
	unsigned char* data;
	int width;
	int height;
} d_img;

typedef struct {
	d_tex2d tex;
} d_font;

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

typedef struct {
	d_tex2d default_tex;
	d_font default_font;
	d_font* cur_font;
	d_tex2d* tex_slots[4];
	d_mesh tri_mesh;
	d_program default_prog;
	d_program* cur_prog;
	mat4 transform;
	d_cam default_cam;
	d_cam* cur_cam;
	mat4 t_stack[8];
	size_t t_stack_cnt;
} d_gfx_t;

extern d_gfx_t d_gfx;

void d_gfx_init();
void d_gfx_frame_start();
void d_gfx_frame_end();

d_mesh d_make_mesh(const d_vertex*, size_t, const unsigned int*, size_t);
d_tex2d d_make_tex(const d_img*);
d_img d_parse_img(const unsigned char*, size_t);
d_img d_make_img(const unsigned char*, int, int);
void d_free_img(d_img*);
d_program d_make_program(const char*, const char*);
d_font d_make_font(d_tex2d);

void d_draw(d_mesh*, d_program*);

void d_send_f(const char*, float);
void d_send_vec2(const char*, vec2);
void d_send_vec3(const char*, vec3);
void d_send_color(const char*, color);
void d_send_mat4(const char*, mat4);

void d_clear();
void d_push();
void d_pop();
void d_move(vec3);
void d_scale(vec3);
void d_rot_x(float);
void d_rot_y(float);
void d_rot_z(float);

#endif

