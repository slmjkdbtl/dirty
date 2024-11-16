#ifndef D_PHYSICS_H
#define D_PHYSICS_H

#ifndef D_MATH_H
#include "d_math.h"
#endif

#define D_MAX_BODIES 2048
#define D_WORLD_GRID_SIZE 64

typedef struct {
	bool active;
	d_poly shape;
	void* userdata;
} d_body;

typedef bool (*d_world_filter)(d_body* a, d_body* b);

typedef struct {
	d_body bodies[D_MAX_BODIES];
	int num_bodies;
	int cur_a;
	int cur_b;
	d_world_filter filter;
} d_world;

typedef struct {
	bool collided;
	d_body* body_a;
	d_body* body_b;
	d_vec2 dis;
} d_world_res;

d_world d_world_new(void);
d_body* d_world_add(d_world* w, d_poly p, void* userdata);
void d_world_rm(d_world* w, d_body* b);
void d_world_check_reset(d_world* w);
d_world_res d_world_check_next(d_world* w);

#endif

#ifdef D_IMPL
#define D_PHYSICS_IMPL
#endif

#ifdef D_PHYSICS_IMPL
#ifndef D_PHYSICS_IMPL_ONCE
#define D_PHYSICS_IMPL_ONCE

d_world d_world_new(void) {
	return (d_world) {
		.bodies = {0},
		.num_bodies = 0,
		.cur_a = 0,
		.cur_b = 0,
		.filter = NULL,
	};
}

d_body* d_world_add(d_world* w, d_poly p, void* userdata) {
	int idx = w->num_bodies;
	w->bodies[w->num_bodies++] = (d_body) {
		.active = true,
		.shape = p,
		.userdata = userdata,
	};
	return &w->bodies[idx];
}

void d_world_rm(d_world* w, d_body* b) {
	for (int i = 0; i < w->num_bodies; i++) {
		if (&w->bodies[i] == b) {
			memmove(
				w->bodies + i,
				w->bodies + i + 1,
				(w->num_bodies - i - 1) * sizeof(d_body)
			);
			w->num_bodies--;
			i--;
			return;
		}
	}
}

void d_world_check_reset(d_world* w) {
	w->cur_a = 0;
	w->cur_b = 0;
}

// TODO: hash grid
d_world_res d_world_check_next(d_world* w) {

	while (true) {

		w->cur_b++;

		if (w->cur_b >= w->num_bodies) {
			w->cur_a++;
			w->cur_b = w->cur_a + 1;
			if (w->cur_a >= w->num_bodies - 1) {
				break;
			}
		}

		d_body* a = &w->bodies[w->cur_a];
		d_body* b = &w->bodies[w->cur_b];

		if (a == b) continue;
		if (!a->active || !b->active) continue;

		if (w->filter) {
			if (w->filter(a, b) == false) continue;
		}

		d_rect a_bbox = d_poly_bbox(a->shape);
		d_rect b_bbox = d_poly_bbox(b->shape);
		if (!d_col_rect_rect(a_bbox, b_bbox)) continue;

		d_vec2 dis = d_vec2f(0, 0);

		if (d_col_sat(a->shape, b->shape, &dis)) {
			return (d_world_res) {
				.collided = true,
				.body_a = a,
				.body_b = b,
				.dis = dis,
			};
		}

	}

	return (d_world_res) {
		.collided = false,
		.body_a = NULL,
		.body_b = NULL,
		.dis = d_vec2f(0, 0),
	};

}

#endif
#endif
