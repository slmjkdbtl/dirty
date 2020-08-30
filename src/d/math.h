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

typedef struct {
	float x;
	float y;
	float z;
} vec3;

vec3 vec3_add(vec3, vec3);
vec3 vec3_sub(vec3, vec3);
vec3 vec3_scale(vec3, float);

typedef struct {
	float r;
	float g;
	float b;
	float a;
} color;

#endif

