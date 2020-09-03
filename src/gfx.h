// wengwengweng

#ifndef D_GFX_H
#define D_GFX_H

#include <stdbool.h>
#include "math.h"
#include "gl.h"

typedef struct {
	mat4 view;
	mat4 proj;
} d_cam;

typedef struct {
	GLuint id;
} d_program;

typedef struct {
	GLuint id;
	int width;
	int height;
} d_tex2d;

typedef struct {
	unsigned char* data;
	int width;
	int height;
} d_img;

typedef struct {
	d_tex2d tex;
	float qw;
	float qh;
	vec2 map[128];
} d_font;

typedef struct {
	GLuint vbuf;
	GLuint ibuf;
	int count;
} d_mesh;

typedef struct {
	vec3 pos;
	vec3 normal;
	vec2 uv;
	color color;
} d_vertex;

typedef struct {
	d_tex2d default_tex;
	d_tex2d* tex_slots[4];
	d_font default_font;
	d_font* cur_font;
	d_mesh tri_mesh;
	d_program default_prog;
	d_program* cur_prog;
	mat4 transform;
	d_cam default_cam;
	d_cam* cur_cam;
	mat4 t_stack[8];
	int t_stack_cnt;
} d_gfx_t;

d_gfx_t d_gfx;

void d_gfx_init();
void d_gfx_frame_start();
void d_gfx_frame_end();

d_mesh d_make_mesh(const d_vertex*, int, const unsigned int*, int);
void d_free_mesh(d_mesh*);

d_img d_parse_img(const unsigned char*, int);
d_img d_make_img(const unsigned char*, int, int);
void d_free_img(d_img*);

d_tex2d d_make_tex(const d_img*);
void d_free_tex(d_tex2d*);

d_font d_make_font(d_tex2d tex, int gw, int gh, const char* chars);
void d_free_font(d_font*);

d_program d_make_program(const char*, const char*);
void d_free_program(d_program*);

void d_draw(d_mesh*, d_program*);

void d_send_f(const char*, float);
void d_send_vec2(const char*, vec2);
void d_send_vec3(const char*, vec3);
void d_send_color(const char*, color);
void d_send_mat4(const char*, mat4);

void d_clear();
void d_clear_color();
void d_clear_depth();
void d_clear_stencil();
void d_depth_write(bool);
void d_depth_test(bool);
void d_stencil_write(bool);
void d_stencil_test(bool);

void d_push();
void d_pop();
void d_move(vec3);
void d_scale(vec3);
void d_rot_x(float);
void d_rot_y(float);
void d_rot_z(float);

#endif

