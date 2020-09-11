// wengwengweng

// math.h
//
// *f() create data specifying each value
// *u() create unit value

#ifndef D_MATH_H
#define D_MATH_H

#include <stdbool.h>

#define WHITE colorf(1.0, 1.0, 1.0, 1.0)
#define BLACK colorf(0.0, 0.0, 0.0, 1.0)
#define RED colorf(1.0, 0.0, 0.0, 1.0)
#define GREEN colorf(0.0, 1.0, 0.0, 1.0)
#define BLUE colorf(0.0, 0.0, 1.0, 1.0)
#define CYAN colorf(0.0, 1.0, 1.0, 1.0)
#define YELLOW colorf(1.0, 1.0, 0.0, 1.0)
#define MAGENTA colorf(1.0, 0.0, 1.0, 1.0)

typedef struct {
	float x;
	float y;
} vec2;

typedef struct {
	float x;
	float y;
	float z;
} vec3;

typedef struct {
	float x;
	float y;
	float z;
	float w;
} vec4;

typedef struct {
	float r;
	float g;
	float b;
	float a;
} color;

typedef struct {
	float m[16];
} mat4;

typedef struct {
	float x;
	float y;
	float z;
	float w;
} quat;

typedef struct {
	float x;
	float y;
	float w;
	float h;
} quad;

vec2 vec2f(float, float);
vec2 vec2u();
vec2 vec2_add(vec2, vec2);
vec2 vec2_sub(vec2, vec2);
vec2 vec2_scale(vec2, float);
vec2 vec2_mult(vec2, vec2);
float vec2_dist(vec2, vec2);
float vec2_len(vec2);
vec2 vec2_unit(vec2);
vec3 vec2_cross(vec2, vec2);
float vec2_dot(vec2, vec2);
vec2 vec2_normal(vec2);
float vec2_angle(vec2, vec2);
vec2 vec2_min(vec2, vec2);
vec2 vec2_max(vec2, vec2);
vec2 vec2_rand(vec2, vec2);
vec2 vec2_lerp(vec2, vec2, float);
vec2 vec2_clamp(vec2, vec2, vec2);
bool vec2_eq(vec2, vec2);
const char* vec2_fmt(vec2);

vec3 vec3f(float, float, float);
vec3 vec3u();
vec3 vec3_add(vec3, vec3);
vec3 vec3_sub(vec3, vec3);
vec3 vec3_scale(vec3, float);
vec3 vec3_mult(vec3, vec3);
float vec3_dist(vec3, vec3);
float vec3_len(vec3);
vec3 vec3_unit(vec3);
vec3 vec3_cross(vec3, vec3);
float vec3_dot(vec3, vec3);
vec3 vec3_min(vec3, vec3);
vec3 vec3_max(vec3, vec3);
vec3 vec3_rand(vec3, vec3);
vec3 vec3_lerp(vec3, vec3, float);
vec3 vec3_clamp(vec3, vec3, vec3);
bool vec3_eq(vec3, vec3);
const char* vec3_fmt(vec3);

vec4 vec4f(float, float, float, float);
vec4 vec4u();

color colorf(float, float, float, float);
color colori(int, float);
color coloru();
color color_invert(color);
color color_darken(color c, float a);
bool color_eq(color, color);
const char* color_fmt(color);

mat4 mat4f(float[16]);
mat4 mat4u();
mat4 mat4_identity();
mat4 mat4_mult(mat4, mat4);
vec4 mat4_mult_vec4(mat4, vec4);
vec3 mat4_mult_vec3(mat4, vec3);
vec2 mat4_mult_vec2(mat4, vec2);
mat4 mat4_invert(mat4);
mat4 mat4_scale(vec3);
mat4 mat4_translate(vec3);
mat4 mat4_ortho(float, float, float, float);
mat4 mat4_proj(float, float, float, float);
mat4 mat4_view(vec3, vec3, vec3);
mat4 mat4_rot_x(float);
mat4 mat4_rot_y(float);
mat4 mat4_rot_z(float);
const char* mat4_fmt(mat4);

quat quatf(float, float, float, float);
quat quatu();

quad quadf(float, float, float, float);
quad quadu();

float degf(float);
float radf(float);
float clampf(float, float, float);
float lerpf(float, float, float);
float randf();
float randf_in(float, float);

bool pt_rect(vec2, vec2, vec2);

#endif

