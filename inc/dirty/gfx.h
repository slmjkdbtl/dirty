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

typedef struct {
	color *pixels;
	int width;
	int height;
} d_img;

d_img d_make_img(int w, int h);
d_img d_parse_img(const unsigned char *bytes, int size);
d_img d_load_img(const char *path);
void d_free_img(d_img *img);

void d_clear();
void d_set_pixel(vec2 p, color c);
color d_get_pixel(vec2 p);
void d_draw_img(const d_img *img, vec2 pos);
void d_draw_tri(vec2 p1, vec2 p2, vec2 p3, color c);
void d_draw_rect(vec2 p1, vec2 p2, color c);
void d_draw_line(vec2 p1, vec2 p2, color c);

#endif

