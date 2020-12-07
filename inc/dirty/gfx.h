// wengwengweng

#ifndef D_GFX_H
#define D_GFX_H

#define D_ASCII_CHARS " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"

#include <stdbool.h>

#include "math.h"

typedef enum {
	D_CENTER,
	D_TOP_LEFT,
	D_TOP,
	D_TOP_RIGHT,
	D_LEFT,
	D_RIGHT,
	D_BOT_LEFT,
	D_BOT,
	D_BOT_RIGHT,
} d_origin;

typedef enum {
	D_ALPHA,
	D_REPLACE,
	D_ADD,
} d_blend;

typedef enum {
	D_BORDER,
	D_EDGE,
	D_REPEAT,
} d_wrap;

typedef struct {
	vec3 pos;
	vec3 normal;
	vec2 uv;
	color color;
} d_vertex;

typedef unsigned int d_index;

typedef struct {
	vec3 pos;
	vec3 scale;
	quat rot;
} d_psr;

typedef struct {
	color *pixels;
	int width;
	int height;
} d_img;

typedef struct {
	d_img *imgs;
	int num_imgs;
} d_imgs;

typedef struct {
	d_imgs imgs;
	int width;
	int height;
	int map[128];
} d_font;

typedef struct {
	d_vertex *verts;
	int num_verts;
	d_index *indices;
	int num_indices;
} d_mesh;

typedef struct d_model_node {
	d_psr psr;
	struct d_model_node *children;
	int num_children;
	d_mesh *meshes;
	int num_meshes;
} d_model_node;

typedef struct {
	d_model_node *nodes;
	int num_nodes;
	d_img *images;
	int num_images;
} d_model;

d_img d_make_img(int w, int h);
d_img d_parse_img(const unsigned char *bytes, int size);
d_img d_load_img(const char *path);
void d_img_set(d_img *img, int x, int y, color c);
void d_img_set_ex(d_img *img, int x, int y, color c, d_blend blend);
color d_img_get(const d_img *img, int x, int y);
color d_img_get_ex(const d_img *img, int x, int y, d_wrap wrap);
void d_img_fill(d_img *img, color c);
void d_img_save(const d_img *img, const char *path);
d_img d_img_clone(const d_img *img);
void d_free_img(d_img *img);
d_imgs d_img_slice(const d_img *img, int w, int h);
void d_free_imgs(d_imgs *imgs);

d_font d_make_font(d_img img, int gw, int gh, const char *chars);
void d_free_font(d_font *font);

void d_free_mesh(d_mesh *mesh);

mat4 d_psr_mat4(d_psr psr);

d_model d_parse_model(const unsigned char *bytes, int size);
d_model d_load_model(const char *path);
void d_free_model(d_model *model);

void d_clear();
void d_set_blend(d_blend b);
void d_set_wrap(d_wrap w);
void d_put(int x, int y, color c);
color d_peek(int x, int y);
void d_draw_img(const d_img *img, vec2 pos);
void d_draw_text(const char *text, vec2 pos);
void d_draw_tri(vec2 p1, vec2 p2, vec2 p3, color c);
void d_draw_rect(vec2 p1, vec2 p2, color c);
void d_draw_circle(vec2 center, float r, color c);
void d_draw_line(vec2 p1, vec2 p2, color c);
void d_draw_mesh(const d_mesh *mesh);
void d_draw_model(const d_model *model);

void d_drawon(d_img *img);
d_img *d_canvas();

#endif

