// wengwengweng

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <dirty/dirty.h>
#include "utils.h"
#include "ui.h"

#define WIDGET_SLOTS 64
#define WINDOW_SLOTS 16

typedef unsigned long d_ui_id;

typedef struct {
	d_ui_id id;
	void* data;
} d_widget;

typedef struct {
	const char* label;
	vec2 pos;
	float width;
	float height;
	d_widget widgets[WIDGET_SLOTS];
	int widget_cnt;
} d_ui_window;

typedef struct {
	d_ui_window* win;
	vec2 dpos;
} d_ui_window_drag;

typedef struct {
	d_ui_window windows[WINDOW_SLOTS];
	int window_cnt;
	d_ui_theme theme;
	d_ui_window* cur_window;
	d_ui_window_drag window_drag;
} d_ui_t;

static d_ui_t d_ui;

void d_ui_init() {

	d_ui.theme = (d_ui_theme) {
		.line_width = 3.0,
		.text_size = 12.0,
		.padding_x = 9.0,
		.padding_y = 6.0,
	};

}

void d_ui_set_theme(d_ui_theme theme) {
	d_ui.theme = theme;
}

static d_ui_window* d_ui_get_window(const char* label, vec2 pos, float w, float h) {

	for (int i = 0; i < d_ui.window_cnt; i++) {
		if (d_ui.windows[i].label == label) {
			return &d_ui.windows[i];
		}
	}

	d_ui.windows[d_ui.window_cnt++] = (d_ui_window) {
		.label = label,
		.pos = pos,
		.width = w,
		.height = h,
		.widgets = {0},
	};

	return &d_ui.windows[d_ui.window_cnt - 1];

}

void d_ui_window_begin(const char* label, vec2 pos, float w, float h) {

	if (d_ui.cur_window) {
		fprintf(stderr, "cannot create window inside window\n");
		d_quit();
	}

	d_ui_window* win = d_ui_get_window(label, pos, w, h);
	d_ui_theme* t = &d_ui.theme;
	vec2 mpos = d_mouse_pos();
	float bar_height = t->padding_y * 2.0 + t->text_size;

	// check drag
	if (d_ui.window_drag.win) {
		if (d_mouse_released(D_MOUSE_LEFT)) {
			d_ui.window_drag.win = NULL;
			d_ui.window_drag.dpos = vec2f(0.0, 0.0);
		} else {
			if (d_ui.window_drag.win == win) {
				win->pos = vec2_add(mpos, d_ui.window_drag.dpos);
			}
		}
	} else {
		if (d_mouse_pressed(D_MOUSE_LEFT)) {
			if (pt_rect(mpos, win->pos, vec2_add(win->pos, vec2f(win->width, -bar_height)))) {
				d_ui.window_drag.win = win;
				d_ui.window_drag.dpos = vec2_sub(win->pos, mpos);
			}
		}
	}

	d_push();
	d_move_xy(win->pos);

	d_draw_lrect(vec2f(0.0, 0.0), vec2f(win->width, -win->height), t->line_width, coloru());
	d_draw_lrect(vec2f(0.0, 0.0), vec2f(win->width, -bar_height), t->line_width, coloru());

	d_push();
	d_move_xy(vec2f(t->padding_x, -t->padding_y));
	d_draw_text(win->label, t->text_size, D_TOP_LEFT, coloru());
	d_pop();

	d_move_xy(vec2f(t->padding_x, -bar_height - t->padding_y));

	d_ui.cur_window = win;

}

void d_ui_window_end() {
	d_ui.cur_window = NULL;
	d_pop();
}

static d_widget* d_ui_get_widget(d_ui_window* win, const char* type, const char* label) {
	d_ui_id id = hash(type) + hash(label);
	for (int i = 0; i < win->widget_cnt; i++) {
		if (id == win->widgets[i].id) {
			return &win->widgets[i];
		}
	}
	win->widgets[win->widget_cnt++] = (d_widget) {
		.id = id,
		.data = NULL,
	};
	return &win->widgets[win->widget_cnt - 1];
}

typedef struct {
	float val;
	bool draggin;
} d_ui_sliderf_t;

// TODO: cleaning
float d_ui_sliderf(const char* label, float start, float min, float max) {

	if (!d_ui.cur_window) {
		fprintf(stderr, "cannot use ui widgets without a window\n");
		d_quit();
	}

	d_ui_theme* t = &d_ui.theme;
	d_ui_window* win = d_ui.cur_window;
	d_widget* w = d_ui_get_widget(d_ui.cur_window, "sliderf", label);

	if (!w->data) {
		w->data = malloc(sizeof(d_ui_sliderf_t));
		d_ui_sliderf_t* data = w->data;
		data->val = start;
		data->draggin = false;
	}

	d_ui_sliderf_t* data = w->data;

	float sw = 12.0;
	float bh = t->text_size;
	float cw = win->width - t->padding_x * 2.0;

	d_draw_text(label, t->text_size, D_TOP_LEFT, coloru());
	d_move_y(-t->text_size - t->padding_y);

	vec2 mpos = mat4_mult_vec2(mat4_invert(d_transform()), d_mouse_pos());
	vec2 b1 = vec2f(0.0, 0.0);
	vec2 b2 = vec2f(cw, -bh);
	float r = (data->val - min) / (max - min);

	if (data->draggin) {
		if (d_mouse_released(D_MOUSE_LEFT)) {
			data->draggin = false;
		} else {
			float dx = d_mouse_dpos().x;
			float dr = dx / cw;
			float df = dr * (max - min);
			data->val += df;
			data->val = clampf(data->val, min, max);
			d_draw_rect(b1, b2, colorf(1.0, 1.0, 1.0, 0.3));
		}
	} else {
		if (d_mouse_pressed(D_MOUSE_LEFT)) {
			if (pt_rect(mpos, b1, b2)) {
				data->draggin = true;
			}
		}
	}

	r = (data->val - min) / (max - min);
	vec2 s1 = vec2f((cw - sw) * r, 0.0);
	vec2 s2 = vec2f(s1.x + sw, -bh);

	d_draw_lrect(b1, b2, t->line_width, coloru());
	d_draw_rect(s1, s2, colorf(1.0, 1.0, 1.0, 1.0));

	d_move_y(-t->text_size - t->padding_y);

	return data->val;

}

void d_ui_text(const char* txt) {

	if (!d_ui.cur_window) {
		fprintf(stderr, "cannot use ui widgets without a window\n");
		d_quit();
	}

	d_ui_theme* t = &d_ui.theme;
	d_draw_text(txt, t->text_size, D_TOP_LEFT, coloru());
	d_move_y(-t->text_size - t->padding_y);

}

typedef struct {
	bool pressed;
} d_ui_button_t;

bool d_ui_button(const char* label) {

	if (!d_ui.cur_window) {
		fprintf(stderr, "cannot use ui widgets without a window\n");
		d_quit();
	}

	d_widget* w = d_ui_get_widget(d_ui.cur_window, "button", label);

	if (!w->data) {
		w->data = malloc(sizeof(d_ui_button_t));
		d_ui_button_t* data = w->data;
		data->pressed = false;
	}

	d_ui_button_t* data = w->data;

	d_ui_theme* t = &d_ui.theme;
	vec2 mpos = mat4_mult_vec2(mat4_invert(d_transform()), d_mouse_pos());
	vec2 p1 = vec2f(0.0, 0.0);
	vec2 p2 = vec2f(p1.x + strlen(label) * t->text_size + t->padding_x * 2.0, p1.y - t->text_size - t->padding_y * 2.0);

	bool in = pt_rect(mpos, p1, p2);

	if (data->pressed) {
		if (d_mouse_released(D_MOUSE_LEFT)) {
			data->pressed = false;
		}
	} else {
		data->pressed = in && d_mouse_pressed(D_MOUSE_LEFT);
	}

	d_draw_lrect(p1, p2, t->line_width, coloru());

	color tcol = colorf(1.0, 1.0, 1.0, 1.0);

	if (data->pressed) {
		d_draw_rect(p1, p2, coloru());
		tcol = colorf(0.0, 0.0, 0.0, 1.0);
	}

	d_push();
	d_move_xy(vec2f(t->padding_x, -t->padding_y));
	d_draw_text(label, t->text_size, D_TOP_LEFT, tcol);
	d_pop();

	d_move_y(p2.y - t->padding_y);

	return in && d_mouse_pressed(D_MOUSE_LEFT);

}

