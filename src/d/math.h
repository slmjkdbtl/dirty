// wengwengweng

#ifndef D_MATH_H
#define D_MATH_H

typedef struct {
	float x;
	float y;
} vec2;

vec2 vec2_add(vec2, vec2);
vec2 vec2_sub(vec2, vec2);
vec2 vec2_scale(vec2, float);
void vec2_print(vec2);

typedef struct {
	float x;
	float y;
	float z;
} vec3;

vec3 vec3_add(vec3, vec3);
vec3 vec3_sub(vec3, vec3);
vec3 vec3_scale(vec3, float);
void vec3_print(vec3);

typedef struct {
	float r;
	float g;
	float b;
	float a;
} color;

typedef struct {
	float m[16];
} mat4;

mat4 make_mat4();
mat4 mat4_mult(mat4, mat4);
mat4 mat4_scale(vec3);
mat4 mat4_translate(vec3);
void mat4_print(mat4);

#endif

