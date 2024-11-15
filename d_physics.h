#ifndef D_PHYSICS_H
#define D_PHYSICS_H

#ifndef D_MATH_H
#include "d_math.h"
#endif

#define D_MAX_BODIES 2048

typedef struct {
	d_poly shape;
	void* userdata;
} d_body;

typedef struct {
	d_body bodies[D_MAX_BODIES];
	int num_bodies;
	int cur_a;
	int cur_b;
} d_world;

typedef struct {
	bool collided;
	d_body* body_a;
	d_body* body_b;
	d_vec2 dis;
} d_world_res;

d_world d_world_new(void);
d_body* d_world_add(d_world* w, d_poly p, void* userdata);
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
	};
}

d_body* d_world_add(d_world* w, d_poly p, void* userdata) {
	int idx = w->num_bodies;
	w->bodies[w->num_bodies++] = (d_body) {
		.shape = p,
		.userdata = userdata,
	};
	return &w->bodies[idx];
}

void d_world_check_reset(d_world* w) {
	w->cur_a = 0;
	w->cur_b = 0;
}

d_world_res d_world_check_next(d_world* w) {
	// TODO: hash grid
	while (true) {
		int i = w->cur_a;
		int j = w->cur_a + w->cur_b + 1;
		d_body* a = &w->bodies[i];
		d_body* b = &w->bodies[j];
		w->cur_b++;
		if (j >= w->num_bodies - 1) {
			w->cur_a++;
			w->cur_b = 0;
			if (w->cur_a >= w->num_bodies - 1) {
				break;
			}
		}
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
