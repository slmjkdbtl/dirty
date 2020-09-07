// wengwengweng

#ifndef D_UI_H
#define D_UI_H

#include "math.h"

typedef struct {
	float line_width;
	float text_size;
	float padding;
} d_ui_theme;

void d_ui_set_theme(d_ui_theme);

void d_ui_window_start(const char* title, vec2 pos, float w, float h);
void d_ui_window_end();

float d_ui_sliderf(const char*, float, float, float);
int d_ui_slideri(const char*, int, int, int);
const char* d_ui_input(const char*);
bool d_ui_button(const char*);
float d_ui_text(const char*);
float d_ui_sep();

#endif

