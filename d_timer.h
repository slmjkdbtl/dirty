#ifndef D_TIMER_H
#define D_TIMER_H

#include <stdbool.h>

typedef struct {
	float elapsed;
	float time;
	bool loop;
} d_timer;

d_timer d_timer_new(float time, bool loop);
bool d_timer_tick(d_timer* t, float dt);
void d_timer_reset(d_timer* t, float time);

#endif

#ifdef D_IMPL
#define D_TIMER_IMPL
#endif

#ifdef D_TIMER_IMPL
#ifndef D_TIMER_IMPL_ONCE
#define D_TIMER_IMPL_ONCE

d_timer d_timer_new(float time, bool loop) {
	return (d_timer) {
		.elapsed = 0,
		.time = time,
		.loop = loop,
	};
}

bool d_timer_tick(d_timer* t, float dt) {
	t->elapsed += dt;
	if (t->elapsed >= t->time) {
		if (t->loop) {
			t->elapsed = t->elapsed - t->time;
		}
		return true;
	}
	return false;
}

void d_timer_reset(d_timer* t, float time) {
	t->elapsed = 0;
}

void d_timer_reset_to(d_timer* t, float time) {
	t->elapsed = 0;
	t->time = time;
}

#endif
#endif
