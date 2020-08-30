// wengwengweng

#include "math.h"

vec2 vec2_add(vec2 a, vec2 b) {
	return (vec2) {
		.x = a.x + b.x,
		.y = a.y + b.y,
	};
}

vec2 vec2_sub(vec2 a, vec2 b) {
	return (vec2) {
		.x = a.x - b.x,
		.y = a.y - b.y,
	};
}

vec2 vec2_scale(vec2 v, float s) {
	return (vec2) {
		.x = v.x * s,
		.y = v.y * s,
	};
}

vec3 vec3_add(vec3 a, vec3 b) {
	return (vec3) {
		.x = a.x + b.x,
		.y = a.y + b.y,
		.z = a.z + b.z,
	};
}

vec3 vec3_sub(vec3 a, vec3 b) {
	return (vec3) {
		.x = a.x - b.x,
		.y = a.y - b.y,
		.z = a.z - b.z,
	};
}

vec3 vec3_scale(vec3 v, float s) {
	return (vec3) {
		.x = v.x * s,
		.y = v.y * s,
		.z = v.z * s,
	};
}

