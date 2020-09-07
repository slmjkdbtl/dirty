// wengwengweng

#include <stdlib.h>

#include <dirty/dirty.h>
#include "utils.h"
#include "ui.h"

#define WIDGET_SLOTS 64
#define WINDOW_SLOTS 8

typedef unsigned long d_ui_id;

typedef struct {
	d_ui_id id;
	void* data;
} d_widget;

typedef struct {
	const char* label;
	d_ui_id id;
	vec2 pos;
	float width;
	float height;
	d_widget widgets[WIDGET_SLOTS];
	int widget_cnt;
} d_ui_window;

typedef struct {
	d_ui_window windows[WINDOW_SLOTS];
	int window_cnt;
	d_ui_theme theme;
	d_ui_window* cur_window;
	float cur_y;
} d_ui_t;

static d_ui_t d_ui;

void d_ui_init() {

	d_ui.theme = (d_ui_theme) {
		.line_width = 3.0,
		.text_size = 12.0,
		.padding = 6.0,
	};

}

void d_ui_set_theme(d_ui_theme theme) {
	d_ui.theme = theme;
}

static d_ui_window* d_ui_get_window(const char* label, vec2 pos, float w, float h) {

	d_ui_id id = hash(label);

	for (int i = 0; i < d_ui.window_cnt; i++) {
		if (d_ui.windows[i].id == id) {
			return &d_ui.windows[i];
		}
	}

	d_ui.windows[d_ui.window_cnt] = (d_ui_window) {
		.label = label,
		.id = id,
		.pos = pos,
		.width = w,
		.height = h,
		.widgets = {0},
	};

	d_ui.window_cnt++;

	return &d_ui.windows[d_ui.window_cnt - 1];

}

void d_ui_window_start(const char* label, vec2 pos, float w, float h) {

	d_ui_window* win = d_ui_get_window(label, pos, w, h);
	d_ui_theme* t = &d_ui.theme;

	float bar_height = t->padding * 2.0 + t->text_size;

	d_push();
	d_move_xy(win->pos);

	d_draw_lrect(vec2f(0.0, 0.0), vec2f(win->width, -win->height), t->line_width);
	d_draw_lrect(vec2f(0.0, 0.0), vec2f(win->width, -bar_height), t->line_width);

	d_push();
	d_move_xy(vec2f(t->padding, -t->padding));
	d_draw_text(win->label, t->text_size, D_TOP_LEFT);
	d_pop();

	d_move_xy(vec2f(t->padding, -bar_height - t->padding));

	d_ui.cur_window = win;
	d_ui.cur_y = 0.0;

}

void d_ui_window_end() {
	d_ui.cur_window = NULL;
	d_ui.cur_y = 0.0;
	d_pop();
}

static void* d_ui_window_get_widget(d_ui_window* win, const char* type, const char* label) {
	d_ui_id id = hash(type) + hash(label);
	for (int i = 0; i < win->widget_cnt; i++) {
		if (id == win->widgets[i].id) {
			return win->widgets[i].data;
		}
	}
	return NULL;
}

float d_ui_sliderf(const char* label, float start, float min, float max) {

	d_ui_theme* t = &d_ui.theme;

	d_push();

	d_draw_text(label, t->text_size, D_TOP_LEFT);
	d_move_y(t->text_size);
// 	d_draw_lrect(vec2f(0.0,));

	d_pop();

}

