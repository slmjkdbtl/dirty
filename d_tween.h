#ifndef D_TWEEN_H
#define D_TWEEN_H

#ifndef D_MATH_H
#include "d_math.h"
#endif

#include <stdlib.h>
#include <stdbool.h>

#define D_MAX_TWEENS 1024

typedef float (*d_ease_func)(float);

typedef struct {
	float from;
	float to;
	float duration;
	d_ease_func easing_func;
	float val;
	float* src_val;
	float elapsed;
	bool paused;
	bool done;
} d_tween;

d_tween d_tween_new(
	float from,
	float to,
	float duration,
	float* src_val,
	d_ease_func func
);

void d_tween_update(d_tween *tween, float dt, float* val);

void d_tween_update_all(float dt);

void d_tween_add_number(
	float from,
	float to,
	float duration,
	float* src_val,
	d_ease_func func
);

void d_tween_add_vec2(
	vec2 from,
	vec2 to,
	float duration,
	vec2* src_val,
	d_ease_func func
);

float d_ease_linear(float t);
float d_ease_in_sine(float t);
float d_ease_out_sine(float t);
float d_ease_in_out_sine(float t);
float d_ease_in_quad(float t);
float d_ease_out_quad(float t);
float d_ease_in_out_quad(float t);
float d_ease_in_cubic(float t);
float d_ease_out_cubic(float t);
float d_ease_in_out_cubic(float t);
float d_ease_in_quart(float t);
float d_ease_out_quart(float t);
float d_ease_in_out_quart(float t);
float d_ease_in_quint(float t);
float d_ease_out_quint(float t);
float d_ease_in_out_quint(float t);
float d_ease_in_etpo(float t);
float d_ease_out_etpo(float t);
float d_ease_in_out_etpo(float t);
float d_ease_in_circ(float t);
float d_ease_out_circ(float t);
float d_ease_in_out_circ(float t);
float d_ease_in_back(float t);
float d_ease_out_back(float t);
float d_ease_in_out_back(float t);
float d_ease_in_elastic(float t);
float d_ease_out_elastic(float t);
float d_ease_in_out_elastic(float t);
float d_ease_in_bounce(float t);
float d_ease_out_bounce(float t);
float d_ease_in_out_bounce(float t);

#endif

#ifdef D_IMPL
#define D_TWEEN_IMPL
#endif

#ifdef D_TWEEN_IMPL
#ifndef D_TWEEN_IMPL_ONCE
#define D_TWEEN_IMPL_ONCE

#define D_PI 3.14

// https://github.com/ai/easings.net/blob/master/src/easings/easingsFunctions.ts
#define C1 1.70158
#define C2 (C1 * 1.525)
#define C3 (C1 + 1)
#define C4 (2 * D_PI) / 3
#define C5 (2 * D_PI) / 4.5

typedef struct {
	d_tween tweens[D_MAX_TWEENS];
	int num_tweens;
} d_tween_man;

static d_tween_man ctx;

d_tween* d_tween_add(d_tween t) {
	int idx = ctx.num_tweens;
	ctx.tweens[ctx.num_tweens++] = t;
	// a bit pointer magic to cancel all previous tween on the value
	for (int i = 0; i < idx; i++) {
		d_tween* t2 = &ctx.tweens[i];
		if (t2->src_val != NULL && t2->src_val == t.src_val) {
			t2->done = true;
		}
	}
	return &ctx.tweens[idx];
}

void d_tween_add_vec2(
	vec2 from,
	vec2 to,
	float duration,
	vec2* src_val,
	d_ease_func func
) {
	d_tween t1 = d_tween_new(from.x, to.x, duration, &src_val->x, func);
	d_tween t2 = d_tween_new(from.y, to.y, duration, &src_val->y, func);
	d_tween_add(t1);
	d_tween_add(t2);
}

void d_tween_update_all(float dt) {
	for (int i = 0; i < ctx.num_tweens; i++) {
		d_tween* t = &ctx.tweens[i];
		if (t->done) {
			ctx.tweens[i] = ctx.tweens[--ctx.num_tweens];
			i--;
			continue;
		}
		d_tween_update(t, dt, NULL);
	}
}

d_tween d_tween_new(
	float from,
	float to,
	float duration,
	float* src_val,
	d_ease_func func
) {
	return (d_tween) {
		.from = from,
		.to = to,
		.duration = duration,
		.easing_func = func == NULL ? d_ease_linear: func,
		.val = from,
		.src_val = src_val,
		.elapsed = 0.0,
		.paused = false,
		.done = false,
	};
}

void d_tween_update(d_tween *tween, float dt, float* val) {
	if (tween->paused || tween->done) return;
	tween->elapsed += dt;
	float t = tween->elapsed / tween->duration;
	if (t >= 1.0f) {
		tween->val = tween->to;
		tween->elapsed = tween->duration;
		tween->done = true;
		if (tween->src_val) *tween->src_val = tween->to;
		if (val) *val = tween->to;
		return;
	}
	tween->val = lerpf(tween->from, tween->to, tween->easing_func(t));
	if (tween->src_val) *tween->src_val = tween->val;
	if (val) *val = tween->val;
}

float d_ease_linear(float t) {
	return t;
}

float d_ease_in_sine(float t) {
	return 1 - cos((t * D_PI) / 2);
}

float d_ease_out_sine(float t) {
	return sin((t * D_PI) / 2);
}

float d_ease_in_out_sine(float t) {
	return -(cos(D_PI * t) - 1) / 2;
}

float d_ease_in_quad(float t) {
	return t * t;
}

float d_ease_out_quad(float t) {
	return 1 - pow(1 - t, 2);
}

float d_ease_in_out_quad(float t) {
	return t < 0.5 ? 2 * t * t : 1 - pow(-2 * t + 2, 2) / 2;
}

float d_ease_in_cubic(float t) {
	return t * t * t;
}

float d_ease_out_cubic(float t) {
	return 1 - pow(1 - t, 3);
}

float d_ease_in_out_cubic(float t) {
	return t < 0.5 ? 4 * t * t * t : 1 - pow(-2 * t + 2, 3) / 2;
}

float d_ease_in_quart(float t) {
	return t * t * t * t;
}

float d_ease_out_quart(float t) {
	return 1 - pow(1 - t, 4);
}

float d_ease_in_out_quart(float t) {
	return t < 0.5 ? 8 * t * t * t * t : 1 - pow(-2 * t + 2, 4) / 2;
}

float d_ease_in_quint(float t) {
	return t * t * t * t * t;
}

float d_ease_out_quint(float t) {
	return 1 - pow(1 - t, 5);
}

float d_ease_in_out_quint(float t) {
	return t < 0.5 ? 16 * t * t * t * t * t : 1 - pow(-2 * t + 2, 5) / 2;
}

float d_ease_in_etpo(float t) {
	return t == 0 ? 0 : pow(2, 10 * t - 10);
}

float d_ease_out_etpo(float t) {
	return t == 1 ? 1 : 1 - pow(2, -10 * t);
}

float d_ease_in_out_etpo(float t) {
	return t == 0
		? 0
		: t == 1
			? 1
			: t < 0.5
				? pow(2, 20 * t - 10) / 2
				: (2 - pow(2, -20 * t + 10)) / 2;
}

float d_ease_in_circ(float t) {
	return 1 - sqrt(1 - pow(t, 2));
}

float d_ease_out_circ(float t) {
	return sqrt(1 - pow(t - 1, 2));
}

float d_ease_in_out_circ(float t) {
	return t < 0.5
		? (1 - sqrt(1 - pow(2 * t, 2))) / 2
		: (sqrt(1 - pow(-2 * t + 2, 2)) + 1) / 2;
}

float d_ease_in_back(float t) {
	return C3 * t * t * t - C1 * t * t;
}

float d_ease_out_back(float t) {
	return 1 + C3 * pow(t - 1, 3) + C1 * pow(t - 1, 2);
}

float d_ease_in_out_back(float t) {
	return t < 0.5
		? (pow(2 * t, 2) * ((C2 + 1) * 2 * t - C2)) / 2
		: (pow(2 * t - 2, 2) * ((C2 + 1) * (t * 2 - 2) + C2) + 2) / 2;
}

float d_ease_in_elastic(float t) {
	return t == 0
		? 0
		: t == 1
			? 1
			: -pow(2, 10 * t - 10) * sin((t * 10 - 10.75) * C4);
}

float d_ease_out_elastic(float t) {
	return t == 0
		? 0
		: t == 1
			? 1
			: pow(2, -10 * t) * sin((t * 10 - 0.75) * C4) + 1;
}

float d_ease_in_out_elastic(float t) {
	return t == 0
		? 0
		: t == 1
			? 1
			: t < 0.5
				? -(pow(2, 20 * t - 10) * sin((20 * t - 11.125) * C5)) / 2
				: (pow(2, -20 * t + 10) * sin((20 * t - 11.125) * C5)) / 2 + 1;
}

float d_ease_in_bounce(float t) {
	return 1 - d_ease_out_bounce(1 - t);
}

// TODO: bugged
float d_ease_out_bounce(float t) {
	float n1 = 7.5625;
	float d1 = 2.75;
	if (t < 1 / d1) {
		return n1 * t * t;
	} else if (t < 2 / d1) {
		t -= 1.5;
		return n1 * (t / d1) * t + 0.75;
	} else if (t < 2.5 / d1) {
		t -= 2.25;
		return n1 * (t / d1) * t + 0.9375;
	} else {
		t -= 2.625;
		return n1 * (t / d1) * t + 0.984375;
	}

}

float d_ease_in_out_bounce(float t) {
	return t < 0.5
		? (1 - d_ease_out_bounce(1 - 2 * t)) / 2
		: (1 + d_ease_out_bounce(2 * t - 1)) / 2;
}

#endif
#endif
