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
	D_REPLACE,
	D_ALPHA,
	D_ADD,
} d_blend;

typedef struct {
	color *pixels;
	int width;
	int height;
} d_img;

typedef struct {
	d_img img;
	int height;
	int width;
	float qw;
	float qh;
	vec2 map[128];
} d_font;

d_img d_make_img(int w, int h);
d_img d_parse_img(const unsigned char *bytes, int size);
d_img d_load_img(const char *path);
void d_img_set(d_img *img, int x, int y, color c);
color d_img_get(const d_img *img, int x, int y);
void d_img_save(const d_img *img, const char *path);
void d_free_img(d_img *img);

d_font d_make_font(d_img img, int gw, int gh, const char *chars);
void d_free_font(d_font *font);

void d_clear();
void d_set_blend(d_blend b);
void d_put(vec2 p, color c);
color d_look(vec2 p);
void d_draw_img(const d_img *img, vec2 pos);
void d_draw_imgq(const d_img *img, quad q, vec2 pos);
void d_draw_tri(vec2 p1, vec2 p2, vec2 p3, color c);
void d_draw_rect(vec2 p1, vec2 p2, color c);
void d_draw_circle(vec2 center, float r, color c);
void d_draw_line(vec2 p1, vec2 p2, color c);

void d_drawon(d_img *img);
d_img *d_canvas();

#endif

