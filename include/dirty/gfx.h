// wengwengweng

#ifndef D_GFX_H
#define D_GFX_H

#include <stdbool.h>
#include "math.h"
#include "gl.h"

typedef enum {
	D_NEAREST = GL_NEAREST,
	D_LINEAR = GL_LINEAR,
} d_tex_filter;

typedef enum {
	D_REPEAT = GL_REPEAT,
	D_MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
	D_CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
	D_CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER,
} d_tex_wrap;

typedef struct {
	mat4 view;
	mat4 proj;
} d_cam;

typedef struct {
	GLuint id;
} d_shader;

typedef struct {
	unsigned char* data;
	int width;
	int height;
} d_img;

typedef struct {
	GLuint id;
	int width;
	int height;
} d_tex;

typedef struct {
	d_tex tex;
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
	GLuint fbuf;
	d_tex ctex;
	GLuint dstex;
	int width;
	int height;
} d_canvas;

typedef struct {
	d_tex_filter filter;
	d_tex_wrap wrap;
} d_tex_conf;

typedef struct {
	vec3 pos;
	vec3 normal;
	vec2 uv;
	color color;
} d_vertex;

typedef unsigned int d_index;

// mesh
d_mesh d_make_mesh(const d_vertex*, int, const d_index*, int);
void d_free_mesh(d_mesh*);

// img
d_img d_parse_img(const unsigned char*, int);
d_img d_load_img(const char*);
void d_free_img(d_img*);

// tex
d_tex d_img_tex(const d_img*);
d_tex d_img_tex_ex(const d_img*, d_tex_conf);
d_tex d_make_tex(const unsigned char*, int, int);
d_tex d_make_tex_ex(const unsigned char*, int, int, d_tex_conf);
d_tex d_parse_tex(const unsigned char*, int);
d_tex d_parse_tex_ex(const unsigned char*, int, d_tex_conf);
d_tex d_load_tex(const char*);
d_tex d_load_tex_ex(const char*, d_tex_conf);
void d_free_tex(d_tex*);
d_tex_conf d_default_tex_conf();

// font
d_font d_make_font(d_tex, int, int, const char*);
void d_free_font(d_font*);

// shader
d_shader d_make_shader(const char*, const char*);
void d_free_shader(d_shader*);

// canvas
d_canvas d_make_canvas(int, int);
d_canvas d_make_canvas_ex(int, int, d_tex_conf);
void d_canvas_capture(const d_canvas*);
void d_free_canvas(d_canvas*);

// uniforms
void d_send_f(const char*, float);
void d_send_vec2(const char*, vec2);
void d_send_vec3(const char*, vec3);
void d_send_color(const char*, color);
void d_send_mat4(const char*, mat4);
void d_send_tex(const char*, int, const d_tex*);

// clear
void d_clear();
void d_clear_color();
void d_clear_depth();
void d_clear_stencil();

// buffer
void d_depth_write(bool);
void d_depth_test(bool);
void d_stencil_write(bool);
void d_stencil_test(bool);

// transform
void d_push();
void d_pop();
void d_move(vec3);
void d_move_x(float);
void d_move_y(float);
void d_move_z(float);
void d_move_xy(vec2);
void d_scale(vec3);
void d_scale_x(float);
void d_scale_y(float);
void d_scale_z(float);
void d_scale_xy(vec2);
void d_rot_x(float);
void d_rot_y(float);
void d_rot_z(float);

// states
void d_use_cam(const d_cam*);
void d_use_shader(const d_shader*);
void d_use_font(const d_font*);
void d_use_canvas(const d_canvas*);

// draw
void d_draw_raw(const d_vertex*, int, const d_index*, int);
void d_draw_mesh(const d_mesh*);
void d_draw_tex(const d_tex*, quad);
void d_draw_text(const char*, float);
void d_draw_canvas(const d_canvas*);
void d_draw_rect(vec2, vec2);
void d_draw_lrect(vec2, vec2, float);
void d_draw_line(vec2, vec2, float);
void d_draw_circle(vec2, float);

#endif

