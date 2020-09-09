// wengwengweng

#ifndef D_UI_H
#define D_UI_H

#include "math.h"

typedef unsigned long d_ui_id;

typedef struct {
	float line_width;
	float text_size;
	float padding_x;
	float padding_y;
	color line_color;
	color bar_color;
	color back_color;
	color text_color;
} d_ui_theme_t;

void d_ui_set_theme(d_ui_theme_t);
const d_ui_theme_t* d_ui_theme();

void d_ui_window_begin(const char* title, vec2 pos, float w, float h);
void d_ui_window_end();

void* d_ui_widget_data(d_ui_id, int, void*);
float d_ui_content_width();

float d_ui_sliderf(const char*, float, float, float);
int d_ui_slideri(const char*, int, int, int);
const char* d_ui_input(const char*);
bool d_ui_button(const char*);
void d_ui_text(const char*);
float d_ui_sep();

#endif

