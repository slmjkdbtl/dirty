// wengwengweng

#include <stdio.h>
#include <math.h>

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

void vec2_print(vec2 v) {
	printf("vec2(%.2f, %.2f)\n", v.x, v.y);
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

void vec3_print(vec3 v) {
	printf("vec3(%.2f, %.2f, %.2f)\n", v.x, v.y, v.z);
}

mat4 make_mat4() {
	return (mat4) {
		.m = {
			1.0, 0.0, 0.0, 0.0,
			0.0, 1.0, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			0.0, 0.0, 0.0, 1.0,
		},
	};
}

mat4 mat4_mult(mat4 m1, mat4 m2) {

	mat4 res = make_mat4();

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			res.m[i * 4 + j] =
				m1.m[0 * 4 + j] * m2.m[i * 4 + 0] +
				m1.m[1 * 4 + j] * m2.m[i * 4 + 1] +
				m1.m[2 * 4 + j] * m2.m[i * 4 + 2] +
				m1.m[3 * 4 + j] * m2.m[i * 4 + 3];
		}
	}

	return res;

}

mat4 mat4_scale(vec3 s) {
	return (mat4) {
		.m = {
			s.x, 0.0, 0.0, 0.0,
			0.0, s.y, 0.0, 0.0,
			0.0, 0.0, s.z, 0.0,
			0.0, 0.0, 0.0, 1.0,
		},
	};
}

mat4 mat4_translate(vec3 p) {
	return (mat4) {
		.m = {
			1.0, 0.0, 0.0, 0.0,
			0.0, 1.0, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			p.x, p.y, p.z, 1.0,
		},
	};
}

mat4 mat4_rot_x(float a) {
	return (mat4) {
		.m = {
			1.0, 0.0, 0.0, 0.0,
			0.0, cos(a), -sin(a), 0.0,
			0.0, sin(a), cos(a), 0.0,
			0.0, 0.0, 0.0, 1.0,
		},
	};
}

mat4 mat4_rot_y(float a) {
	return (mat4) {
		.m = {
			cos(a), 0.0, -sin(a), 0.0,
			0.0, 1.0, 0.0, 0.0,
			sin(a), 0.0, cos(a), 0.0,
			0.0, 0.0, 0.0, 1.0,
		},
	};
}

mat4 mat4_rot_z(float a) {
	return (mat4) {
		.m = {
			cos(a), -sin(a), 0.0, 0.0,
			sin(a), cos(a), 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			0.0, 0.0, 0.0, 1.0,
		},
	};
}

void mat4_print(mat4 m) {
	printf(
		"mat4(%.2f, %.2f, %.2f, %.2f,\n"
		"     %.2f, %.2f, %.2f, %.2f,\n"
		"     %.2f, %.2f, %.2f, %.2f,\n"
		"     %.2f, %.2f, %.2f, %.2f)\n",
		m.m[0], m.m[1], m.m[2], m.m[3],
		m.m[4], m.m[5], m.m[6], m.m[7],
		m.m[8], m.m[9], m.m[10], m.m[11],
		m.m[12], m.m[13], m.m[14], m.m[15]
	);
}

