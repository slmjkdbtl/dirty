#ifndef D_MATH_H
#define D_MATH_H

#include "lua.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define D_RNG_A 1103515245
#define D_RNG_C 12345
#define D_RNG_M 2147483648
#define D_MAX_POLY_VERTS 8

#define D_CLEAR ((d_color) { 0, 0, 0, 0 })
#define D_WHITE ((d_color) { 255, 255, 255, 255 })
#define D_BLACK ((d_color) { 0, 0, 0, 255 })
#define D_RED   ((d_color) { 255, 0, 0, 255 })
#define D_GREEN ((d_color) { 0, 255, 0, 255 })
#define D_BLUE  ((d_color) { 0, 0, 255, 255 })

typedef struct {
	uint64_t seed;
} d_rng;

typedef struct {
	float x;
	float y;
} d_vec2;

typedef struct {
	float x;
	float y;
	float z;
} d_vec3;

typedef struct {
	float x;
	float y;
	float z;
	float w;
} d_vec4;

typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
} d_color;

typedef struct {
	float h;
	float s;
	float l;
	uint8_t a;
} d_hsl;

typedef struct {
	float m[16];
} d_mat4;

typedef struct {
	float x;
	float y;
	float z;
	float w;
} d_quat;

typedef struct {
	float x;
	float y;
	float w;
	float h;
} d_quad;

typedef struct {
	d_vec2 p1;
	d_vec2 p2;
} d_rect;

typedef struct {
	d_vec3 p1;
	d_vec3 p2;
} d_box;

typedef struct {
	d_vec2 p1;
	d_vec2 p2;
} d_line2;

typedef struct {
	d_vec3 p1;
	d_vec3 p2;
} d_line3;

typedef struct {
	d_vec2 verts[D_MAX_POLY_VERTS];
	int num_verts;
} d_poly;

typedef struct {
	d_vec2 center;
	float radius;
} d_circle;

typedef struct {
	d_vec2 center;
	float radius_x;
	float radius_y;
	float angle;
} d_ellipse;

typedef struct {
	d_vec3 center;
	float radius;
} d_sphere;

typedef struct {
	d_vec3 normal;
	float dist;
} d_plane;

typedef struct {
	d_vec2 origin;
	d_vec2 dir;
} d_ray2;

typedef struct {
	d_vec3 origin;
	d_vec3 dir;
} d_ray3;

d_rng d_rng_new(uint64_t seed);
float d_rng_gen(d_rng* rng);
float d_rng_rand(d_rng* rng, float low, float hi);
int d_rng_randi(d_rng* rng, int low, int hi);
float d_randf(float, float);
int d_randi(int, int);
bool d_chance(float);
float d_wave(float a, float b, float t);

d_vec2 d_vec2f(float, float);
d_vec2 d_vec2u(void);
d_vec2 d_vec2_add(d_vec2, d_vec2);
d_vec2 d_vec2_sub(d_vec2, d_vec2);
d_vec2 d_vec2_scale(d_vec2, float);
d_vec2 d_vec2_mult(d_vec2, d_vec2);
float d_vec2_dist(d_vec2, d_vec2);
float d_vec2_len(d_vec2);
d_vec2 d_vec2_unit(d_vec2);
d_vec3 d_vec2_cross(d_vec2, d_vec2);
float d_vec2_dot(d_vec2, d_vec2);
d_vec2 d_vec2_normal(d_vec2);
float d_vec2_angle(d_vec2, d_vec2);
d_vec2 d_vec2_min(d_vec2, d_vec2);
d_vec2 d_vec2_max(d_vec2, d_vec2);
d_vec2 d_vec2_rand(d_vec2, d_vec2);
d_vec2 d_vec2_lerp(d_vec2, d_vec2, float);
d_vec2 d_vec2_clamp(d_vec2, d_vec2, d_vec2);
bool d_vec2_eq(d_vec2, d_vec2);
void d_vec2_swap(d_vec2*, d_vec2*);

d_vec3 d_vec3f(float, float, float);
d_vec3 d_vec3u(void);
d_vec3 d_vec3_add(d_vec3, d_vec3);
d_vec3 d_vec3_sub(d_vec3, d_vec3);
d_vec3 d_vec3_scale(d_vec3, float);
d_vec3 d_vec3_mult(d_vec3, d_vec3);
float d_vec3_dist(d_vec3, d_vec3);
float d_vec3_len(d_vec3);
d_vec3 d_vec3_unit(d_vec3);
d_vec3 d_vec3_cross(d_vec3, d_vec3);
float d_vec3_dot(d_vec3, d_vec3);
d_vec3 d_vec3_min(d_vec3, d_vec3);
d_vec3 d_vec3_max(d_vec3, d_vec3);
d_vec3 d_vec3_rand(d_vec3, d_vec3);
d_vec3 d_vec3_lerp(d_vec3, d_vec3, float);
d_vec3 d_vec3_clamp(d_vec3, d_vec3, d_vec3);
bool d_vec3_eq(d_vec3, d_vec3);
void d_vec3_swap(d_vec3*, d_vec3*);

d_vec4 d_vec4f(float, float, float, float);
d_vec4 d_vec4u(void);

d_color d_coloru(void);
d_color d_colori(uint8_t, uint8_t, uint8_t, uint8_t);
d_color d_colorf(float, float, float, float);
d_color d_colorx(uint32_t hex);
d_color d_color_mix(d_color c1, d_color c2);
d_color d_color_blend(d_color c1, d_color c2);
d_color d_color_darken(d_color c, int d);
d_color d_color_lighten(d_color c, int l);
bool d_color_eq(d_color, d_color);
d_color d_color_lerp(d_color from, d_color to, float t);
d_hsl d_rgb2hsl(d_color rgb);
d_color d_hsl2rgb(d_hsl hsl);
bool d_hsl_eq(d_hsl c1, d_hsl c2);

d_mat4 d_mat4u(void);
d_mat4 d_mat4_identity(void);
d_mat4 d_mat4_mult(d_mat4, d_mat4);
d_vec4 d_mat4_mult_vec4(d_mat4, d_vec4);
d_vec3 d_mat4_mult_vec3(d_mat4, d_vec3);
d_vec2 d_mat4_mult_vec2(d_mat4, d_vec2);
d_mat4 d_mat4_invert(d_mat4);
d_mat4 d_mat4_transpose(d_mat4 mat);
d_mat4 d_mat4_scale(d_vec3);
d_mat4 d_mat4_translate(d_vec3);
d_mat4 d_mat4_ortho(float w, float n, float near, float far);
d_mat4 d_mat4_persp(float fov, float aspect, float far, float near);
d_mat4 d_mat4_view(d_vec3 pos, d_vec3 dir, d_vec3 up);
d_mat4 d_mat4_rot_x(float);
d_mat4 d_mat4_rot_y(float);
d_mat4 d_mat4_rot_z(float);
d_mat4 d_mat4_rot_quat(d_quat);
d_mat4 d_mat4_lerp(d_mat4 m1, d_mat4 m2, float t);

d_quat d_quatu(void);
d_quat d_quat_mult(d_quat q1, d_quat q2);

d_quad d_quadu(void);

void d_rect_fix(d_rect* r);
float d_rect_area(d_rect r);
d_poly d_rect_to_poly(d_rect r);
d_poly d_rect_transform(d_rect r, d_mat4 t);

d_poly d_poly_transform(d_poly p, d_mat4 t);
d_rect d_poly_bbox(d_poly p);

bool d_col_pt_pt(d_vec2 p1, d_vec2 p2);
bool d_col_pt_line(d_vec2 p, d_line2 l);
bool d_col_pt_rect(d_vec2 pt, d_rect r);
bool d_col_pt_circle(d_vec2 pt, d_circle c);
bool d_col_pt_poly(d_vec2 l, d_poly p);
bool d_col_line_line(d_line2 l1, d_line2 l2);
bool d_col_line_rect(d_line2 l, d_rect r);
bool d_col_line_circle(d_line2 l, d_circle c);
bool d_col_line_poly(d_line2 l, d_poly p);
bool d_col_circle_circle(d_circle c1, d_circle c2);
bool d_col_circle_rect(d_circle c, d_rect r);
bool d_col_circle_poly(d_circle c, d_poly p);
bool d_col_rect_rect(d_rect r1, d_rect r2);
bool d_col_rect_poly(d_rect r, d_poly p);
bool d_col_poly_poly(d_poly p1, d_poly p2);

bool d_col_sat(d_poly p1, d_poly p2, d_vec2* dis);

float d_deg2rad(float);
float d_rad2deg(float);
float d_clampf(float, float, float);
int d_clampi(int, int, int);
float d_lerpf(float, float, float);
float d_mapf(float, float, float, float, float);
int d_mapi(int, int, int, int, int);
void d_swapi(int*, int*);
void d_swapf(float*, float*);

#ifdef D_SHORTCUT
d_vec2 v2(float x, float y);
d_vec3 v3(float x, float y, float z);
d_color rgb(float r, float g, float b);
d_color rgba(float r, float g, float b, float a);
float r(float a, float b);
#endif

#endif

#if defined(D_MATH_IMPL) || defined(D_IMPL)
#ifndef D_MATH_IMPL_ONCE
#define D_MATH_IMPL_ONCE

d_rng d_rng_new(uint64_t seed) {
	return (d_rng) {
		.seed = seed,
	};
}

float d_rng_gen(d_rng* rng) {
	rng->seed = (D_RNG_A * rng->seed + D_RNG_C) % D_RNG_M;
	return (float)(rng->seed) / (float)D_RNG_M;
}

float d_rng_rand(d_rng* rng, float low, float hi) {
	return low + d_rng_gen(rng) * (hi - low);
}

int d_rng_randi(d_rng* rng, int low, int hi) {
	return (int)d_rng_rand(rng, low, hi);
}

float d_randf(float low, float hi) {
	return low + (float)rand() / (float)RAND_MAX * (hi - low);
}

int d_randi(int low, int hi) {
	return (int)d_randf(low, hi);
}

bool d_chance(float c) {
	return d_randf(0, 1) <= c;
}

float d_wave(float a, float b, float t) {
	return a + ((sinf(t) + 1) / 2) * (b - a);
}

d_vec2 d_vec2f(float x, float y) {
	return (d_vec2) {
		.x = x,
		.y = y,
	};
}

d_vec2 d_vec2u(void) {
	return (d_vec2) { 0.0, 0.0 };
}

d_vec2 d_vec2_add(d_vec2 p1, d_vec2 p2) {
	return (d_vec2) {
		.x = p1.x + p2.x,
		.y = p1.y + p2.y,
	};
}

d_vec2 d_vec2_sub(d_vec2 p1, d_vec2 p2) {
	return (d_vec2) {
		.x = p1.x - p2.x,
		.y = p1.y - p2.y,
	};
}

d_vec2 d_vec2_scale(d_vec2 p, float s) {
	return (d_vec2) {
		.x = p.x * s,
		.y = p.y * s,
	};
}

d_vec2 d_vec2_mult(d_vec2 p1, d_vec2 p2) {
	return (d_vec2) {
		.x = p1.x * p2.x,
		.y = p1.y * p2.y,
	};
}

float d_vec2_dist(d_vec2 p1, d_vec2 p2) {
	return sqrt(
		(p1.x - p2.x) * (p1.x - p2.x) +
		(p1.y - p2.y) * (p1.y - p2.y)
	);
}

float d_vec2_len(d_vec2 p) {
	return d_vec2_dist(p, (d_vec2) { 0.0, 0.0 });
}

d_vec2 d_vec2_unit(d_vec2 p) {
	return d_vec2_scale(p, 1.0 / d_vec2_len(p));
}

d_vec3 d_vec2_cross(d_vec2 p1, d_vec2 p2) {
	return d_vec3_cross(d_vec3f(p1.x, p1.y, 0.0), d_vec3f(p2.x, p2.y, 0.0));
}

float d_vec2_dot(d_vec2 p1, d_vec2 p2) {
	return p1.x * p2.x + p1.y * p2.y;
}

d_vec2 d_vec2_normal(d_vec2 p) {
	return (d_vec2) { p.y, -p.x };
}

float d_vec2_angle(d_vec2 p1, d_vec2 p2) {
	return atan2f(p1.y - p2.y, p1.x - p2.x);
}

d_vec2 d_vec2_min(d_vec2 a, d_vec2 b) {
	return (d_vec2) {
		.x = fminf(a.x, b.x),
		.y = fminf(a.y, b.y),
	};
}

d_vec2 d_vec2_max(d_vec2 a, d_vec2 b) {
	return (d_vec2) {
		.x = fmaxf(a.x, b.x),
		.y = fmaxf(a.y, b.y),
	};
}

d_vec2 d_vec2_lerp(d_vec2 from, d_vec2 to, float t) {
	return (d_vec2) {
		.x = d_lerpf(from.x, to.x, t),
		.y = d_lerpf(from.y, to.y, t),
	};
}

d_vec2 d_vec2_rand(d_vec2 p1, d_vec2 p2) {
	return (d_vec2) {
		.x = d_randf(p1.x, p2.x),
		.y = d_randf(p1.y, p2.y),
	};
}

d_vec2 d_vec2_clamp(d_vec2 p, d_vec2 low, d_vec2 hi) {
	return (d_vec2) {
		.x = d_clampf(p.x, low.x, hi.x),
		.y = d_clampf(p.x, low.y, hi.y),
	};
}

bool d_vec2_eq(d_vec2 p1, d_vec2 p2) {
	return p1.x == p2.x && p1.y == p2.y;
}

void d_vec2_swap(d_vec2* p1, d_vec2* p2) {
	d_vec2 p3 = *p2;
	*p2 = *p1;
	*p1 = p3;
}

d_vec3 d_vec3f(float x, float y, float z) {
	return (d_vec3) {
		.x = x,
		.y = y,
		.z = z,
	};
}

d_vec3 d_vec3u(void) {
	return (d_vec3) { 0.0, 0.0, 0.0 };
}

d_vec3 d_vec3_add(d_vec3 p1, d_vec3 p2) {
	return (d_vec3) {
		.x = p1.x + p2.x,
		.y = p1.y + p2.y,
		.z = p1.z + p2.z,
	};
}

d_vec3 d_vec3_sub(d_vec3 p1, d_vec3 p2) {
	return (d_vec3) {
		.x = p1.x - p2.x,
		.y = p1.y - p2.y,
		.z = p1.z - p2.z,
	};
}

d_vec3 d_vec3_scale(d_vec3 p, float s) {
	return (d_vec3) {
		.x = p.x * s,
		.y = p.y * s,
		.z = p.z * s,
	};
}

d_vec3 d_vec3_mult(d_vec3 p1, d_vec3 p2) {
	return (d_vec3) {
		.x = p1.x * p2.x,
		.y = p1.y * p2.y,
		.z = p1.z * p2.z,
	};
}

float d_vec3_dist(d_vec3 p1, d_vec3 p2) {
	return sqrt(
		(p1.x - p2.x) * (p1.x - p2.x) +
		(p1.y - p2.y) * (p1.y - p2.y) +
		(p1.z - p2.z) * (p1.z - p2.z)
	);
}

float d_vec3_len(d_vec3 p) {
	return d_vec3_dist(p, d_vec3f(0.0, 0.0, 0.0));
}

d_vec3 d_vec3_unit(d_vec3 p) {
	return d_vec3_scale(p, 1.0 / d_vec3_len(p));
}

d_vec3 d_vec3_cross(d_vec3 p1, d_vec3 p2) {
	return (d_vec3) {
		.x = (p1.y * p2.z) - (p1.z * p2.y),
		.y = (p1.z * p2.x) - (p1.x * p2.z),
		.z = (p1.x * p2.y) - (p1.y * p2.x),
	};
}

float d_vec3_dot(d_vec3 p1, d_vec3 p2) {
	return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}

d_vec3 d_vec3_min(d_vec3 a, d_vec3 b) {
	return (d_vec3) {
		.x = fminf(a.x, b.x),
		.y = fminf(a.y, b.y),
		.z = fminf(a.z, b.z),
	};
}

d_vec3 d_vec3_max(d_vec3 a, d_vec3 b) {
	return (d_vec3) {
		.x = fmaxf(a.x, b.x),
		.y = fmaxf(a.y, b.y),
		.z = fmaxf(a.z, b.z),
	};
}

d_vec3 d_vec3_lerp(d_vec3 from, d_vec3 to, float t) {
	return (d_vec3) {
		.x = d_lerpf(from.x, to.x, t),
		.y = d_lerpf(from.y, to.y, t),
		.z = d_lerpf(from.z, to.z, t),
	};
}

d_vec3 d_vec3_rand(d_vec3 p1, d_vec3 p2) {
	return (d_vec3) {
		.x = d_randf(p1.x, p2.x),
		.y = d_randf(p1.y, p2.y),
		.z = d_randf(p1.z, p2.z),
	};
}

d_vec3 d_vec3_clamp(d_vec3 p, d_vec3 low, d_vec3 hi) {
	return (d_vec3) {
		.x = d_clampf(p.x, low.x, hi.x),
		.y = d_clampf(p.x, low.y, hi.y),
		.z = d_clampf(p.x, low.z, hi.z),
	};
}

bool d_vec3_eq(d_vec3 p1, d_vec3 p2) {
	return p1.x == p2.x && p1.y == p2.y && p1.z == p2.z;
}

void d_vec3_swap(d_vec3* p1, d_vec3* p2) {
	d_vec3 p3 = *p2;
	*p2 = *p1;
	*p1 = p3;
}

d_vec4 d_vec4f(float x, float y, float z, float w) {
	return (d_vec4) {
		.x = x,
		.y = y,
		.z = z,
		.w = w,
	};
}

d_vec4 d_vec4u(void) {
	return (d_vec4) { 0.0, 0.0, 0.0, 1.0 };
}

d_color d_colori(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	return (d_color) {
		.r = r,
		.g = g,
		.b = b,
		.a = a,
	};
}

d_color d_colorf(float r, float g, float b, float a) {
	return (d_color) {
		.r = r * 255,
		.g = g * 255,
		.b = b * 255,
		.a = a * 255,
	};
}

d_color d_colorx(uint32_t hex) {
	return (d_color) {
		.r = ((hex >> 24) & 0xff),
		.g = ((hex >> 16) & 0xff),
		.b = ((hex >> 8) & 0xff),
		.a = ((hex >> 0) & 0xff),
	};
}

d_color d_coloru(void) {
	return (d_color) {
		.r = 255,
		.g = 255,
		.b = 255,
		.a = 255,
	};
}

d_color d_color_mix(d_color c1, d_color c2) {
	return (d_color) {
		.r = c1.r * c2.r / 255,
		.g = c1.g * c2.g / 255,
		.b = c1.b * c2.b / 255,
		.a = c1.a * c2.a / 255,
	};
}

d_color d_color_blend(d_color c1, d_color c2) {
	float a1 = c1.a / 255.0;
	float a2 = c2.a / 255.0;
	float a = a1 + a2 * (1.0f - a1);
	if (a == 0) return (d_color){ 0, 0, 0, 0 };
	float r = c1.r * a1 + c2.r * a2 * (1.0 - a1);
	float g = c1.g * a1 + c2.g * a2 * (1.0 - a1);
	float b = c1.b * a1 + c2.b * a2 * (1.0 - a1);
	return (d_color) {
		.r = (uint8_t)(r + 0.5),
		.g = (uint8_t)(g + 0.5),
		.b = (uint8_t)(b + 0.5),
		.a = (uint8_t)(a * 255 + 0.5),
	};
}

d_color d_color_invert(d_color c) {
	return (d_color) {
		.r = 255 - c.r,
		.g = 255 - c.g,
		.b = 255 - c.b,
		.a = c.a,
	};
}

d_color d_color_darken(d_color c, int d) {
	return (d_color) {
		.r = d_clampi(c.r - d, 0, 255),
		.g = d_clampi(c.g - d, 0, 255),
		.b = d_clampi(c.b - d, 0, 255),
		.a = c.a,
	};
}

d_color d_color_lighten(d_color c, int l) {
	return d_color_darken(c, -l);
}

bool d_color_eq(d_color c1, d_color c2) {
	return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b && c1.a == c2.a;
}

d_color d_color_lerp(d_color from, d_color to, float t) {
	return (d_color) {
		.r = (int)d_lerpf(from.r, to.r, t),
		.g = (int)d_lerpf(from.g, to.g, t),
		.b = (int)d_lerpf(from.b, to.b, t),
		.a = (int)d_lerpf(from.a, to.a, t),
	};
}

d_hsl d_rgb2hsl(d_color rgb) {

	float rf = rgb.r / 255.0;
	float gf = rgb.g / 255.0;
	float bf = rgb.b / 255.0;
	float max = fmax(fmax(rf, gf), bf);
	float min = fmin(fmin(rf, gf), bf);
	float delta = max - min;
	float l = (max + min) / 2.0;

	if (delta == 0) {
		return (d_hsl) { 0, 0, l, rgb.a };
	}

	float s = l > 0.5
		? delta / (2.0 - max - min)
		: delta / (max + min);

	float h = 0;

	if (max == rf) {
		h = fmod((gf - bf) / delta + (gf < bf ? 6.0 : 0.0), 6.0);
	} else if (max == gf) {
		h = ((bf - rf) / delta) + 2.0;
	} else {
		h = ((rf - gf) / delta) + 4.0;
	}

	h *= 60.0;

	return (d_hsl) { h, s, l, rgb.a };

}

float d_hue2rgb(float p, float q, float t) {
	if (t < 0)
		t += 1;
	if (t > 1)
		t -= 1;
	if (t < 1.0 / 6)
		return p + (q - p) * 6 * t;
	if (t < 1.0 / 2)
		return q;
	if (t < 2.0 / 3)
		return p + (q - p) * (2.0 / 3 - t) * 6;
	return p;
}

d_color d_hsl2rgb(d_hsl hsl) {
	float h = hsl.h;
	float s = hsl.s;
	float l = hsl.l;
	uint8_t a = hsl.a;
	if(hsl.s == 0) {
		return (d_color) { l, l, l, a };
	}
	float q = l < 0.5 ? l * (1 + s) : l + s - l * s;
	float p = 2 * l - q;
	return (d_color) {
		.r = d_hue2rgb(p, q, h + 1.0 / 3) * 255,
		.g = d_hue2rgb(p, q, h) * 255,
		.b = d_hue2rgb(p, q, h - 1.0 / 3) * 255,
		.a = a,
	};
}

bool d_hsl_eq(d_hsl c1, d_hsl c2) {
	return c1.h == c2.h && c1.s == c2.s && c1.l == c2.l && c1.a == c2.a;
}

d_mat4 d_mat4u(void) {
	return d_mat4_identity();
}

d_mat4 d_mat4_identity(void) {
	return (d_mat4) {
		.m = {
			1.0, 0.0, 0.0, 0.0,
			0.0, 1.0, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			0.0, 0.0, 0.0, 1.0,
		},
	};
}

d_mat4 d_mat4_mult(d_mat4 a, d_mat4 b) {
	d_mat4 out = {0};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			out.m[i * 4 + j] =
				a.m[0 * 4 + j] * b.m[i * 4 + 0] +
				a.m[1 * 4 + j] * b.m[i * 4 + 1] +
				a.m[2 * 4 + j] * b.m[i * 4 + 2] +
				a.m[3 * 4 + j] * b.m[i * 4 + 3];
		}
	}
	return out;
}

d_vec4 d_mat4_mult_vec4(d_mat4 m, d_vec4 p) {
	return (d_vec4) {
		.x = p.x * m.m[0] + p.y * m.m[4] + p.z * m.m[8] + p.w * m.m[12],
		.y = p.x * m.m[1] + p.y * m.m[5] + p.z * m.m[9] + p.w * m.m[13],
		.z = p.x * m.m[2] + p.y * m.m[6] + p.z * m.m[10] + p.w * m.m[14],
		.w = p.x * m.m[3] + p.y * m.m[7] + p.z * m.m[11] + p.w * m.m[15]
	};
}

d_vec3 d_mat4_mult_vec3(d_mat4 m, d_vec3 p) {
	d_vec4 p4 = d_mat4_mult_vec4(m, d_vec4f(p.x, p.y, p.z, 1.0));
	return (d_vec3) { p4.x, p4.y, p4.z };
}

d_vec2 d_mat4_mult_vec2(d_mat4 m, d_vec2 p) {
	d_vec3 p3 = d_mat4_mult_vec3(m, d_vec3f(p.x, p.y, 0.0));
	return (d_vec2) { p3.x, p3.y };
}

#define DAMPING_FACTOR 1e-6

// tikhonov regularization
static d_mat4 d_mat4_invert_damped(d_mat4 mat) {
	d_mat4 At = d_mat4_transpose(mat);
	d_mat4 AtA = d_mat4_mult(At, mat);
	for (int i = 0; i < 4; i++) {
		AtA.m[i * 4 + i] += DAMPING_FACTOR;
	}
	d_mat4 AtA_inv = d_mat4_invert(AtA);
	return d_mat4_mult(AtA_inv, At);
}

d_mat4 d_mat4_invert(d_mat4 in) {

	d_mat4 out = {0};
	float* m = in.m;
	float inv[16];
	float det;
	int i;

	out.m[0] =
		m[5]  * m[10] * m[15] -
		m[5]  * m[11] * m[14] -
		m[9]  * m[6]  * m[15] +
		m[9]  * m[7]  * m[14] +
		m[13] * m[6]  * m[11] -
		m[13] * m[7]  * m[10];

	out.m[4] =
		-m[4] * m[10] * m[15] +
		m[4]  * m[11] * m[14] +
		m[8]  * m[6]  * m[15] -
		m[8]  * m[7]  * m[14] -
		m[12] * m[6]  * m[11] +
		m[12] * m[7]  * m[10];

	out.m[8] =
		m[4]  * m[9]  * m[15] -
		m[4]  * m[11] * m[13] -
		m[8]  * m[5]  * m[15] +
		m[8]  * m[7]  * m[13] +
		m[12] * m[5]  * m[11] -
		m[12] * m[7]  * m[9];

	out.m[12] =
		-m[4] * m[9]  * m[14] +
		m[4]  * m[10] * m[13] +
		m[8]  * m[5]  * m[14] -
		m[8]  * m[6]  * m[13] -
		m[12] * m[5]  * m[10] +
		m[12] * m[6]  * m[9];

	out.m[1] =
		-m[1] * m[10] * m[15] +
		m[1]  * m[11] * m[14] +
		m[9]  * m[2]  * m[15] -
		m[9]  * m[3]  * m[14] -
		m[13] * m[2]  * m[11] +
		m[13] * m[3]  * m[10];

	out.m[5] =
		m[0]  * m[10] * m[15] -
		m[0]  * m[11] * m[14] -
		m[8]  * m[2]  * m[15] +
		m[8]  * m[3]  * m[14] +
		m[12] * m[2]  * m[11] -
		m[12] * m[3]  * m[10];

	out.m[9] =
		-m[0] * m[9]  * m[15] +
		m[0]  * m[11] * m[13] +
		m[8]  * m[1]  * m[15] -
		m[8]  * m[3]  * m[13] -
		m[12] * m[1]  * m[11] +
		m[12] * m[3]  * m[9];

	out.m[13] =
		m[0]  * m[9]  * m[14] -
		m[0]  * m[10] * m[13] -
		m[8]  * m[1]  * m[14] +
		m[8]  * m[2]  * m[13] +
		m[12] * m[1]  * m[10] -
		m[12] * m[2]  * m[9];

	out.m[2] =
		m[1]  * m[6] * m[15] -
		m[1]  * m[7] * m[14] -
		m[5]  * m[2] * m[15] +
		m[5]  * m[3] * m[14] +
		m[13] * m[2] * m[7] -
		m[13] * m[3] * m[6];

	out.m[6] =
		-m[0] * m[6] * m[15] +
		m[0]  * m[7] * m[14] +
		m[4]  * m[2] * m[15] -
		m[4]  * m[3] * m[14] -
		m[12] * m[2] * m[7] +
		m[12] * m[3] * m[6];

	out.m[10] =
		m[0]  * m[5] * m[15] -
		m[0]  * m[7] * m[13] -
		m[4]  * m[1] * m[15] +
		m[4]  * m[3] * m[13] +
		m[12] * m[1] * m[7] -
		m[12] * m[3] * m[5];

	out.m[14] =
		-m[0] * m[5] * m[14] +
		m[0]  * m[6] * m[13] +
		m[4]  * m[1] * m[14] -
		m[4]  * m[2] * m[13] -
		m[12] * m[1] * m[6] +
		m[12] * m[2] * m[5];

	out.m[3] =
		-m[1] * m[6] * m[11] +
		m[1]  * m[7] * m[10] +
		m[5]  * m[2] * m[11] -
		m[5]  * m[3] * m[10] -
		m[9]  * m[2] * m[7] +
		m[9]  * m[3] * m[6];

	out.m[7] =
		m[0] * m[6] * m[11] -
		m[0] * m[7] * m[10] -
		m[4] * m[2] * m[11] +
		m[4] * m[3] * m[10] +
		m[8] * m[2] * m[7] -
		m[8] * m[3] * m[6];

	out.m[11] =
		-m[0] * m[5] * m[11] +
		m[0]  * m[7] * m[9] +
		m[4]  * m[1] * m[11] -
		m[4]  * m[3] * m[9] -
		m[8]  * m[1] * m[7] +
		m[8]  * m[3] * m[5];

	out.m[15] =
		m[0] * m[5] * m[10] -
		m[0] * m[6] * m[9] -
		m[4] * m[1] * m[10] +
		m[4] * m[2] * m[9] +
		m[8] * m[1] * m[6] -
		m[8] * m[2] * m[5];

	det = m[0] * out.m[0] + m[1] * out.m[4] + m[2] * out.m[8] + m[3] * out.m[12];

	if (det == 0) {
		return d_mat4_invert_damped(in);
	}

	det = 1.0 / det;

	for (i = 0; i < 16; i++)
		out.m[i] *= det;

	return out;

}

d_mat4 d_mat4_transpose(d_mat4 mat) {
	float* m = mat.m;
	return (d_mat4) {
		.m = {
			m[0], m[4], m[8], m[12],
			m[1], m[5], m[9], m[13],
			m[2], m[6], m[10], m[14],
			m[3], m[7], m[11], m[15],
		},
	};
}

d_mat4 d_mat4_translate(d_vec3 p) {
	return (d_mat4) {
		.m = {
			1.0, 0.0, 0.0, 0.0,
			0.0, 1.0, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			p.x, p.y, p.z, 1.0,
		},
	};
}

d_mat4 d_mat4_scale(d_vec3 s) {
	return (d_mat4) {
		.m = {
			s.x, 0.0, 0.0, 0.0,
			0.0, s.y, 0.0, 0.0,
			0.0, 0.0, s.z, 0.0,
			0.0, 0.0, 0.0, 1.0,
		},
	};
}

d_mat4 d_mat4_rot_x(float a) {
	a = d_deg2rad(a);
	float s = sinf(a);
	float c = cosf(a);
	return (d_mat4) {
		.m = {
			1.0, 0.0, 0.0, 0.0,
			0.0, c, s, 0.0,
			0.0, -s, c, 0.0,
			0.0, 0.0, 0.0, 1.0,
		},
	};
}

d_mat4 d_mat4_rot_y(float a) {
	a = d_deg2rad(a);
	float s = sinf(a);
	float c = cosf(a);
	return (d_mat4) {
		.m = {
			c, 0.0, -s, 0.0,
			0.0, 1.0, 0.0, 0.0,
			s, 0.0, c, 0.0,
			0.0, 0.0, 0.0, 1.0,
		},
	};
}

d_mat4 d_mat4_rot_z(float a) {
	a = d_deg2rad(a);
	float s = sinf(a);
	float c = cosf(a);
	return (d_mat4) {
		.m = {
			c, s, 0.0, 0.0,
			-s, c, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			0.0, 0.0, 0.0, 1.0,
		},
	};
}

d_mat4 d_mat4_rot_quat(d_quat q) {
	float xx = q.x * q.x;
	float yy = q.y * q.y;
	float zz = q.z * q.z;
	float xy = q.x * q.y;
	float xz = q.x * q.z;
	float yz = q.y * q.z;
	float wx = q.w * q.x;
	float wy = q.w * q.y;
	float wz = q.w * q.z;
	return (d_mat4) {
		.m = {
			1.0f - 2.0f * (yy + zz), 2.0f * (xy + wz), 2.0f * (xz - wy), 0.0f,
			2.0f * (xy - wz), 1.0f - 2.0f * (xx + zz), 2.0f * (yz + wx), 0.0f,
			2.0f * (xz + wy), 2.0f * (yz - wx), 1.0f - 2.0f * (xx + yy), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		},
	};
}

d_mat4 d_mat4_lerp(d_mat4 m1, d_mat4 m2, float t) {
	d_mat4 m3;
	for (int i = 0; i < 16; i++) {
		m3.m[i] = d_lerpf(m1.m[i], m2.m[i], t);
	}
	return m3;
}

d_mat4 d_mat4_ortho(float w, float h, float near, float far) {

	float left = -w / 2.0;
	float right = w / 2.0;
	float bottom = -h / 2.0;
	float top = h / 2.0;
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	float tz = -(far + near) / (far - near);

	return (d_mat4) {
		.m = {
			2.0 / (right - left), 0.0, 0.0, 0.0,
			0.0, 2.0 / (top - bottom), 0.0, 0.0,
			0.0, 0.0, -2.0 / (far - near), 0.0,
			tx, ty, tz, 1.0,
		},
	};

}

d_mat4 d_mat4_persp(float fov, float aspect, float near, float far) {
	float f = 1.0 / tanf(d_deg2rad(fov) / 2.0);
	return (d_mat4) {
		.m = {
			-f / aspect, 0.0, 0.0,                                0.0,
			0.0,         f,   0.0,                                0.0,
			0.0,         0.0, (far + near) / (far - near),        1.0,
			0.0,         0.0, -(2.0 * far * near) / (far - near), 0.0,
		}
	};
}

d_mat4 d_mat4_lookat(d_vec3 eye, d_vec3 center, d_vec3 up) {

	d_vec3 z = d_vec3_unit(d_vec3_sub(center, eye));
	d_vec3 x = d_vec3_unit(d_vec3_cross(up, z));
	d_vec3 y = d_vec3_cross(z, x);

	return (d_mat4) {
		.m = {
			x.x, y.x, z.x, 0.0,
			x.y, y.y, z.y, 0.0,
			x.z, y.z, z.z, 0.0,
			-d_vec3_dot(x, eye), -d_vec3_dot(y, eye), -d_vec3_dot(z, eye), 1.0,
		},
	};

}

d_quat d_quatu(void) {
	return (d_quat) { 0.0, 0.0, 0.0, 1.0 };
}

d_quat d_quat_mult(d_quat q1, d_quat q2) {
	return (d_quat) {
		q1.x * q2.x,
		q1.y * q2.y,
		q1.z * q2.z,
		q1.w * q2.w
	};
}

d_quad d_quadu(void) {
	return (d_quad) { 0.0, 0.0, 1.0, 1.0 };
}

void d_rect_fix(d_rect* r) {
	d_vec2 pp1 = d_vec2_min(r->p1, r->p2);
	d_vec2 pp2 = d_vec2_max(r->p1, r->p2);
	r->p1 = pp1;
	r->p2 = pp2;
}

d_poly d_rect_to_poly(d_rect r) {
	return (d_poly) {
		.num_verts = 4,
		.verts = {
			r.p1,
			(d_vec2) { r.p2.x, r.p1.y },
			r.p2,
			(d_vec2) { r.p1.x, r.p2.y },
		},
	};
}

d_poly d_rect_transform(d_rect r, d_mat4 t) {
	return d_poly_transform(d_rect_to_poly(r), t);
}

d_poly d_poly_transform(d_poly p, d_mat4 t) {
	for (int i = 0; i < p.num_verts; i++) {
		p.verts[i] = d_mat4_mult_vec2(t, p.verts[i]);
	}
	return p;
}

d_rect d_poly_bbox(d_poly p) {
	d_vec2 p1 = d_vec2f(FLT_MAX, FLT_MAX);
	d_vec2 p2 = d_vec2f(-FLT_MAX, -FLT_MAX);
	for (int i = 0; i < p.num_verts; i++) {
		d_vec2 pt = p.verts[i];
		p1.x = fminf(p1.x, pt.x);
		p2.x = fmaxf(p2.x, pt.x);
		p1.y = fminf(p1.y, pt.y);
		p2.y = fmaxf(p2.y, pt.y);
	}
	return (d_rect) { p1, p2 };
}

bool d_col_pt_pt(d_vec2 p1, d_vec2 p2) {
	return p1.x == p2.x && p1.y == p2.y;
}

bool d_col_pt_line(d_vec2 p, d_line2 l) {
	float cross =
		(p.y - l.p1.y) * (l.p2.x - l.p1.x)
		- (p.x - l.p1.x) * (l.p2.y - l.p1.y);
	if (cross != 0) {
		return false;
	}
	if (d_col_pt_rect(p, (d_rect) { l.p1, l.p2 })) {
		return false;
	}
	return true;
}

bool d_col_pt_circle(d_vec2 pt, d_circle c) {
	return d_vec2_dist(pt, c.center) <= c.radius;
}

bool d_col_pt_rect(d_vec2 pt, d_rect r) {
	d_rect_fix(&r);
	return
		pt.x >= r.p1.x
		&& pt.x <= r.p2.x
		&& pt.y >= r.p1.y
		&& pt.y <= r.p2.y;
}

bool d_col_pt_poly(d_vec2 pt, d_poly poly) {
	bool c = false;
	d_vec2* p = poly.verts;
	for (int i = 0, j = poly.num_verts - 1; i < poly.num_verts; j = i++) {
		if (
			((p[i].y > pt.y) != (p[j].y > pt.y))
			&& (pt.x < (p[j].x - p[i].x) * (pt.y - p[i].y) / (p[j].y - p[i].y) + p[i].x)
		) {
			c = !c;
		}
	}
	return c;
}

bool d_col_line_line(d_line2 l1, d_line2 l2) {
	float a =
		(
			(l2.p2.x - l2.p1.x)
			* (l1.p1.y - l2.p1.y)
			- (l2.p2.y - l2.p1.y)
			* (l1.p1.x - l2.p1.x)
		)
		/
		(
			(l2.p2.y - l2.p1.y)
			* (l1.p2.x - l1.p1.x)
			- (l2.p2.x - l2.p1.x)
			* (l1.p2.y - l1.p1.y)
		);
	float b =
		(
			(l1.p2.x - l1.p1.x)
			* (l1.p1.y - l2.p1.y)
			- (l1.p2.y - l1.p1.y)
			* (l1.p1.x - l2.p1.x)
		)
		/
		(
			(l2.p2.y - l2.p1.y)
			* (l1.p2.x - l1.p1.x)
			- (l2.p2.x - l2.p1.x)
			* (l1.p2.y - l1.p1.y)
		);
	return a >= 0.0 && a <= 1.0 && b >= 0.0 && b <= 1.0;
}

bool d_col_line_circle(d_line2 l, d_circle c) {
	// vector from p1 to p2
	float abx = l.p2.x - l.p1.x;
	float aby = l.p2.y - l.p1.y;
	// vector from p1 to circle center
	float acx = c.center.x - l.p1.x;
	float acy = c.center.y - l.p1.y;
	// project circle center onto line segment, computing the parameter t along
	float ab_ab = abx * abx + aby * aby;
	float ac_ab = acx * abx + acy * aby;
	float t = d_clampf(ac_ab / ab_ab, 0.0f, 1.0f);
	// find the closest pt on the line segment to the circle center
	float close_x = l.p1.x + t * abx;
	float close_y = l.p1.y + t * aby;
	// calc dis from the closest pt to the circle center
	float dx = close_x - c.center.x;
	float dy = close_y - c.center.y;
	float dis_sq = dx * dx + dy * dy;
	// compare distance to radius squared
	return dis_sq <= c.radius * c.radius;
}

bool d_col_line_rect(d_line2 l, d_rect r) {
	if (d_col_pt_rect(l.p1, r) || d_col_pt_rect(l.p2, r)) {
		return true;
	}
	return
		d_col_line_line(l, (d_line2) { r.p1, (d_vec2) { r.p2.x, r.p1.y } })
		|| d_col_line_line(l, (d_line2) { (d_vec2) { r.p2.x, r.p1.y }, r.p2 })
		|| d_col_line_line(l, (d_line2) { r.p2, (d_vec2) { r.p1.x, r.p2.y } })
		|| d_col_line_line(l, (d_line2) { (d_vec2) { r.p1.x, r.p2.y }, r.p1 });
}

bool d_col_line_poly(d_line2 l, d_poly p) {
	if (d_col_pt_poly(l.p1, p) || d_col_pt_poly(l.p2, p)) {
		return true;
	}
	for (int i = 0; i < p.num_verts; i++) {
		d_vec2 p1 = p.verts[i];
		d_vec2 p2 = p.verts[(i + 1) % p.num_verts];
		d_line2 l2 = { p1, p2 };
		if (d_col_line_line(l, l2)) return true;
	}
	return false;
}

bool d_col_circle_circle(d_circle c1, d_circle c2) {
	return d_vec2_dist(c1.center, c2.center) <= c1.radius + c2.radius;
}

bool d_col_circle_rect(d_circle c, d_rect r) {
	d_rect_fix(&r);
	float closest_x = fmaxf(r.p1.x, fminf(c.center.x, r.p2.x));
	float closest_y = fmaxf(r.p1.y, fminf(c.center.y, r.p2.y));
	float dis_x = c.center.x - closest_x;
	float dis_y = c.center.y - closest_y;
	float dis_s = dis_x * dis_x + dis_y * dis_y;
	return dis_s <= (c.radius * c.radius);
}

bool d_col_circle_poly(d_circle c, d_poly p) {
	if (d_col_pt_circle(p.verts[0], c)) return true;
	if (d_col_pt_poly(c.center, p)) return true;
	for (int i = 0; i < p.num_verts; i++) {
		d_vec2 pt1 = p.verts[i];
		d_vec2 pt2 = p.verts[(i + 1) % p.num_verts];
		d_line2 l = { pt1, pt2 };
		if (d_col_line_circle(l, c)) return true;
	}
	return false;
}

bool d_col_rect_rect(d_rect r1, d_rect r2) {
	d_rect_fix(&r1);
	d_rect_fix(&r2);
	return
		r1.p2.x >= r2.p1.x
		&& r1.p1.x <= r2.p2.x
		&& r1.p2.y >= r2.p1.y
		&& r1.p1.y <= r2.p2.y;
}

bool d_col_rect_poly(d_rect r, d_poly p) {
	return d_col_poly_poly(d_rect_to_poly(r), p);
}

bool d_col_poly_poly(d_poly p1, d_poly p2) {
	for (int i = 0; i < p1.num_verts; i++) {
		d_vec2 pt1 = p1.verts[i];
		d_vec2 pt2 = p1.verts[(i + 1) % p1.num_verts];
		d_line2 l = { pt1, pt2 };
		if (d_col_line_poly(l, p2)) return true;
	}
	return false;
}

bool d_col_sat(d_poly p1, d_poly p2, d_vec2* dis_out) {
	float overlap = FLT_MAX;
	d_vec2 dis = { 0, 0 };
	d_poly polys[2] = { p1, p2 };
	for (int i = 0; i < 2; i++) {
		d_poly p = polys[i];
		for (int i = 0; i < p.num_verts; i++) {
			d_vec2 pt1 = p.verts[i];
			d_vec2 pt2 = p.verts[(i + 1) % p.num_verts];
			d_vec2 axis_proj = d_vec2_unit(d_vec2_normal(d_vec2_sub(pt2, pt1)));
			float min1 = FLT_MAX;
			float max1 = -FLT_MAX;
			for (int j = 0; j < p1.num_verts; j++) {
				float q = d_vec2_dot(p1.verts[j], axis_proj);
				min1 = fminf(min1, q);
				max1 = fmaxf(max1, q);
			}
			float min2 = FLT_MAX;
			float max2 = -FLT_MAX;
			for (int j = 0; j < p2.num_verts; j++) {
				float q = d_vec2_dot(p2.verts[j], axis_proj);
				min2 = fminf(min2, q);
				max2 = fmaxf(max2, q);
			}
			float o = fminf(max1, max2) - fmaxf(min1, min2);
			if (o < 0) {
				if (dis_out) *dis_out = (d_vec2) { 0, 0 };
				return false;
			}
			if (o < fabsf(overlap)) {
				float o1 = max2 - min1;
				float o2 = min2 - max1;
				overlap = fabsf(o1) < fabsf(o2) ? o1 : o2;
				dis = d_vec2_scale(axis_proj, overlap);
			}
		}
	}
	if (dis_out) *dis_out = dis;
	return true;
}

float d_rad2deg(float r) {
	return r * (180.0 / M_PI);
}

float d_deg2rad(float d) {
	return d * (M_PI / 180.0);
}

int d_maxi(int a, int b) {
	return a > b ? a : b;
}

int d_mini(int a, int b) {
	return a < b ? a : b;
}

float d_clampf(float v, float low, float hi) {
	return fmaxf(low, fminf(v, hi));
}

int d_clampi(int v, int low, int hi) {
	return d_maxi(low, d_mini(v, hi));
}

float d_lerpf(float a, float b, float t) {
	return a + (b - a) * t;
}

float d_mapf(float v, float l1, float h1, float l2, float h2) {
	return l2 + (v - l1) * (h2 - l2) / (h1 - l1);
}

int d_mapi(int v, int l1, int h1, int l2, int h2) {
	return l2 + (v - l1) * (h2 - l2) / (h1 - l1);
}

void d_swapi(int* a, int* b) {
	int c = *a;
	*a = *b;
	*b = c;
}

void d_swapf(float* a, float* b) {
	float c = *a;
	*a = *b;
	*b = c;
}

#ifdef D_SHORTCUT
d_vec2 v2(float x, float y) {
	return (d_vec2) {
		.x = x,
		.y = y,
	};
}

d_vec3 v3(float x, float y, float z) {
	return (d_vec3) {
		.x = x,
		.y = y,
		.z = z,
	};
}

d_color rgb(float r, float g, float b) {
	return (d_color) {
		.r = r,
		.g = g,
		.b = b,
		.a = 255,
	};
}

d_color rgba(float r, float g, float b, float a) {
	return (d_color) {
		.r = r,
		.g = g,
		.b = b,
		.a = a,
	};
}

float r(float a, float b) {
	return d_randf(a, b);
}
#endif

#endif
#endif
