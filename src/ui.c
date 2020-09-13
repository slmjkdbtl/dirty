// wengwengweng

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <dirty/dirty.h>
#include "ui.h"

#define WIDGET_SLOTS 64
#define WINDOW_SLOTS 16

typedef struct {
	d_ui_id id;
	void *data;
} d_widget;

typedef struct {
	const char *label;
	vec2 pos;
	float width;
	float height;
	d_widget widgets[WIDGET_SLOTS];
	int widget_cnt;
	int flags;
} d_ui_window;

typedef struct {
	d_ui_window *win;
	vec2 dpos;
} d_ui_window_drag;

typedef struct {
	d_ui_window windows[WINDOW_SLOTS];
	int window_cnt;
	d_ui_theme_t theme;
	d_ui_window *cur_window;
	d_ui_window_drag window_drag;
	bool wants_mouse;
	bool wants_key;
} d_ui_t;

static d_ui_t d_ui;

void d_ui_init() {

	d_ui.theme = (d_ui_theme_t) {
		.line_width = 3.0,
		.text_size = 12.0,
		.padding_x = 9.0,
		.padding_y = 6.0,
		.line_color = WHITE,
		.bar_color = BLACK,
		.back_color = BLACK,
		.text_color = WHITE,
	};

}

void d_ui_set_theme(d_ui_theme_t theme) {
	d_ui.theme = theme;
}

const d_ui_theme_t *d_ui_theme() {
	return &d_ui.theme;
}

static d_ui_window *d_ui_get_window(const char *label, vec2 pos, float w, float h, int flags) {

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
		.flags = flags,
	};

	return &d_ui.windows[d_ui.window_cnt - 1];

}

void d_ui_begin(const char *label, vec2 pos, float w, float h, int flags) {

	if (d_ui.cur_window) {
		d_fail("cannot create window inside window\n");
	}

	d_ui_window *win = d_ui_get_window(label, pos, w, h, flags);
	const d_ui_theme_t *t = d_ui_theme();
	vec2 mpos = d_mouse_pos();
	float bar_height = t->padding_y * 2.0 + t->text_size;

	// check drag
	if (win->flags & D_UI_MOVABLE) {
		if (d_ui.window_drag.win) {
			if (d_mouse_released(D_MOUSE_LEFT)) {
				d_ui.wants_mouse = false;
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
					d_ui.wants_mouse = true;
					d_ui.window_drag.win = win;
					d_ui.window_drag.dpos = vec2_sub(win->pos, mpos);
				}
			}
		}
	}

	d_push();
	d_move_xy(win->pos);

	d_draw_rect(vec2f(0.0, 0.0), vec2f(win->width, -win->height), t->back_color);
	d_draw_rect(vec2f(0.0, 0.0), vec2f(win->width, -bar_height), t->bar_color);
	d_draw_lrect(vec2f(0.0, 0.0), vec2f(win->width, -win->height), t->line_width, t->line_color);
	d_draw_lrect(vec2f(0.0, 0.0), vec2f(win->width, -bar_height), t->line_width, t->line_color);

	d_push();
	d_move_xy(vec2f(t->padding_x, -t->padding_y));
	d_draw_text(win->label, t->text_size, 0.0, D_TOP_LEFT, t->text_color);
	d_pop();

	d_move_xy(vec2f(t->padding_x, -bar_height - t->padding_y));

	d_ui.cur_window = win;

}

void d_ui_end() {
	d_ui.cur_window = NULL;
	d_pop();
}

// TODO: use actual hashmap
void *d_ui_widget_data(d_ui_id id, int size, void *init_data) {

	if (!d_ui.cur_window) {
		d_fail("cannot use ui widgets without a ui window\n");
	}

	for (int i = 0; i < d_ui.cur_window->widget_cnt; i++) {
		if (id == d_ui.cur_window->widgets[i].id) {
			return d_ui.cur_window->widgets[i].data;
		}
	}

	d_ui.cur_window->widgets[d_ui.cur_window->widget_cnt++] = (d_widget) {
		.id = id,
		.data = malloc(size),
	};

	void *data = d_ui.cur_window->widgets[d_ui.cur_window->widget_cnt - 1].data;

	memcpy(data, init_data, size);

	return data;

}

float d_ui_content_width() {
	return d_ui.cur_window->width - d_ui.theme.padding_x * 2.0;
}

typedef struct {
	float val;
	bool draggin;
} d_ui_sliderf_t;

float d_ui_sliderf(const char *label, float start, float min, float max) {

	d_ui_id id = d_hash_str("sliderf") + d_hash_str(label);
	const d_ui_theme_t *t = d_ui_theme();
	float cw = d_ui_content_width();

	d_ui_sliderf_t *data = d_ui_widget_data(id, sizeof(d_ui_sliderf_t), &(d_ui_sliderf_t) {
		.val = start,
		.draggin = false,
	});

	float sw = 12.0;
	float bh = t->text_size;

	d_draw_text(label, t->text_size, cw, D_TOP_LEFT, t->text_color);
	d_move_y(-t->text_size - t->padding_y);

	vec2 mpos = d_mouse_pos_t();
	vec2 b1 = vec2f(0.0, 0.0);
	vec2 b2 = vec2f(cw, -bh);
	float r = (data->val - min) / (max - min);

	if (data->draggin) {
		if (d_mouse_released(D_MOUSE_LEFT)) {
			d_ui.wants_mouse = false;
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
				d_ui.wants_mouse = true;
				data->draggin = true;
			}
		}
	}

	r = (data->val - min) / (max - min);
	vec2 s1 = vec2f((cw - sw) * r, 0.0);
	vec2 s2 = vec2f(s1.x + sw, -bh);

	d_draw_lrect(b1, b2, t->line_width, t->line_color);
	d_draw_rect(s1, s2, t->text_color);

	d_move_y(-t->text_size - t->padding_y);

	return data->val;

}

void d_ui_text(const char *txt) {

	const d_ui_theme_t *t = d_ui_theme();
	float cw = d_ui_content_width();
	d_draw_text(txt, t->text_size, cw, D_TOP_LEFT, t->text_color);
	d_move_y(-t->text_size - t->padding_y);

}

typedef struct {
	bool pressed;
} d_ui_button_t;

bool d_ui_button(const char *label) {

	d_ui_id id = d_hash_str("button") + d_hash_str(label);
	const d_ui_theme_t *t = d_ui_theme();
	float cw = d_ui_content_width();

	d_ui_button_t *data = d_ui_widget_data(id, sizeof(d_ui_button_t), &(d_ui_button_t) {
		.pressed = false,
	});

	vec2 mpos = d_mouse_pos_t();
	vec2 p1 = vec2f(0.0, 0.0);
	vec2 p2 = vec2f(p1.x + strlen(label) * t->text_size + t->padding_y * 2.0, p1.y - t->text_size - t->padding_y * 2.0);

	bool hover = pt_rect(mpos, p1, p2);

	if (data->pressed) {
		if (d_mouse_released(D_MOUSE_LEFT)) {
			data->pressed = false;
			d_ui.wants_mouse = false;
		}
	} else {
		if (hover) {
			if (d_mouse_pressed(D_MOUSE_LEFT)) {
				data->pressed = true;
				d_ui.wants_mouse = true;
			}
		}
	}

	d_draw_lrect(p1, p2, t->line_width, t->line_color);

	color tcol = t->text_color;

	if (data->pressed) {
		d_draw_rect(p1, p2, t->text_color);
		tcol = colorf(0.0, 0.0, 0.0, 1.0);
	}

	d_push();
	d_move_xy(vec2f(t->padding_y, -t->padding_y));
	d_draw_text(label, t->text_size, cw, D_TOP_LEFT, tcol);
	d_pop();

	d_move_y(p2.y - t->padding_y);

	return hover && d_mouse_pressed(D_MOUSE_LEFT);

}

typedef struct {
	char buf[256];
	bool focused;
} d_ui_input_t;

void d_ui_sep() {
	const d_ui_theme_t *t = d_ui_theme();
	d_move_y(-t->padding_y);
	d_draw_line(vec2f(0.0, 0.0), vec2f(d_ui_content_width(), 0.0), t->line_width, t->line_color);
	d_move_y(-t->padding_y);
}

void d_ui_space(float y) {
	d_move_y(-y);
}

const char* d_ui_input(const char *label) {

	d_ui_id id = d_hash_str("button") + d_hash_str(label);
	const d_ui_theme_t *t = d_ui_theme();
	float cw = d_ui_content_width();

	d_ui_input_t *data = d_ui_widget_data(id, sizeof(d_ui_input_t), &(d_ui_input_t) {
		.buf = "",
		.focused = false,
	});

	d_draw_text(label, t->text_size, cw, D_TOP_LEFT, t->text_color);
	d_move_y(-t->padding_y - t->text_size);

	vec2 mpos = d_mouse_pos_t();
	float bh = t->text_size + t->padding_y * 2.0;
	vec2 b1 = vec2f(0.0, 0.0);
	vec2 b2 = vec2f(cw, -bh);

	if (d_mouse_pressed(D_MOUSE_LEFT)) {
		data->focused = pt_rect(mpos, b1, b2);
	}

	d_ui.wants_key = data->focused;

	if (data->focused) {

		const char *tinput = d_tinput();

		if (strlen(tinput) > 0) {
			strcat(data->buf, tinput);
		}

		if (d_key_pressed(D_KEY_BACKSPACE)) {
			data->buf[strlen(data->buf) - 1] = '\0';
		}

		d_draw_rect(b1, b2, colorf(1.0, 1.0, 1.0, 0.3));

	}

	d_draw_lrect(b1, b2, t->line_width, t->line_color);

	d_push();
	d_move_xy(vec2f(t->padding_y, -t->padding_y));
	d_ftext ftext = d_fmt_text(data->buf, t->text_size, 0.0, D_TOP_LEFT, t->text_color);
	vec2 cpos = vec2_add(d_ftext_cpos(&ftext, strlen(data->buf)), vec2f(3.0, 0.0));
	d_draw_ftext(&ftext);
	if (data->focused) {
		d_draw_line(cpos, vec2f(cpos.x, cpos.y - t->text_size), t->line_width, t->line_color);
	}
	d_pop();

	d_move_y(b2.y - t->padding_y);

	return data->buf;

}

bool d_ui_wants_mouse() {
	return d_ui.wants_mouse;
}

bool d_ui_wants_key() {
	return d_ui.wants_key;
}

