// wengwengweng

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "math.h"

vec2 make_vec2(float x, float y) {
	return (vec2) {
		.x = x,
		.y = y,
	};
}

vec2 vec2_add(vec2 p1, vec2 p2) {
	return (vec2) {
		.x = p1.x + p2.x,
		.y = p1.y + p2.y,
	};
}

vec2 vec2_sub(vec2 p1, vec2 p2) {
	return (vec2) {
		.x = p1.x - p2.x,
		.y = p1.y - p2.y,
	};
}

vec2 vec2_scale(vec2 p, float s) {
	return (vec2) {
		.x = p.x * s,
		.y = p.y * s,
	};
}

float vec2_dist(vec2 p1, vec2 p2) {
	return sqrt(
		(p1.x - p2.x) * (p1.x - p2.x) +
		(p1.y - p2.y) * (p1.y - p2.y)
	);
}

float vec2_len(vec2 p) {
	return vec2_dist(p, make_vec2(0.0, 0.0));
}

vec2 vec2_unit(vec2 p) {
	return vec2_scale(p, 1.0 / vec2_len(p));
}

vec3 vec2_cross(vec2 p1, vec2 p2) {
	return vec3_cross(make_vec3(p1.x, p1.y, 0.0), make_vec3(p2.x, p2.y, 0.0));
}

float vec2_dot(vec2 p1, vec2 p2) {
	return p1.x * p2.x + p1.y * p2.y;
}

bool vec2_eq(vec2 p1, vec2 p2) {
	return p1.x == p2.x && p1.y == p2.y;
}

void vec2_print(vec2 p) {
	printf("vec2(%.8g, %.8g)\n", p.x, p.y);
}

vec3 make_vec3(float x, float y, float z) {
	return (vec3) {
		.x = x,
		.y = y,
		.z = z,
	};
}

vec3 vec3_add(vec3 p1, vec3 p2) {
	return (vec3) {
		.x = p1.x + p2.x,
		.y = p1.y + p2.y,
		.z = p1.z + p2.z,
	};
}

vec3 vec3_sub(vec3 p1, vec3 p2) {
	return (vec3) {
		.x = p1.x - p2.x,
		.y = p1.y - p2.y,
		.z = p1.z - p2.z,
	};
}

vec3 vec3_scale(vec3 p, float s) {
	return (vec3) {
		.x = p.x * s,
		.y = p.y * s,
		.z = p.z * s,
	};
}

float vec3_dist(vec3 p1, vec3 p2) {
	return sqrt(
		(p1.x - p2.x) * (p1.x - p2.x) +
		(p1.y - p2.y) * (p1.y - p2.y) +
		(p1.z - p2.z) * (p1.z - p2.z)
	);
}

float vec3_len(vec3 p) {
	return vec3_dist(p, (vec3) { 0.0, 0.0, 0.0, });
}

vec3 vec3_unit(vec3 p) {
	return vec3_scale(p, 1.0 / vec3_len(p));
}

vec3 vec3_cross(vec3 p1, vec3 p2) {
	return (vec3) {
		.x = (p1.y * p2.z) - (p1.z * p2.y),
		.y = (p1.z * p2.x) - (p1.x * p2.z),
		.z = (p1.x * p2.y) - (p1.y * p2.x),
	};
}

float vec3_dot(vec3 p1, vec3 p2) {
	return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}

bool vec3_eq(vec3 p1, vec3 p2) {
	return p1.x == p2.x && p1.y == p2.y && p1.z == p2.z;
}

void vec3_print(vec3 p) {
	printf("vec3(%.8g, %.8g, %.8g)\n", p.x, p.y, p.z);
}

color make_color(float r, float g, float b, float a) {
	return (color) {
		.r = r,
		.g = g,
		.b = b,
		.a = a,
	};
}

bool color_eq(color c1, color c2) {
	return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b && c1.a == c2.a;
}

void color_print(color c) {
	printf("color(%.8g, %.8g, %.8g, %.8g)\n", c.r, c.g, c.b, c.a);
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

mat4 mat4_ortho(float w, float h, float near, float far) {

	float left = -w / 2.0;
	float right = w / 2.0;
	float bottom = -h / 2.0;
	float top = h / 2.0;
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	float tz = -(far + near) / (far - near);

	return (mat4) {
		.m = {
			2.0 / (right - left), 0.0, 0.0, 0.0,
			0.0, 2.0 / (top - bottom), 0.0, 0.0,
			0.0, 0.0, -2.0 / (far - near), 0.0,
			tx, ty, tz, 1.0,
		},
	};

}

mat4 mat4_view(vec3 pos, vec3 dir, vec3 up) {

	vec3 z = vec3_unit(dir);
	vec3 x = vec3_unit(vec3_cross(up, z));
	vec3 y = vec3_cross(z, x);

	return (mat4) {
		.m = {
			x.x, y.x, z.x, 0.0,
			x.y, y.y, z.y, 0.0,
			x.z, y.z, z.z, 0.0,
			-vec3_dot(x, pos), -vec3_dot(y, pos), -vec3_dot(z, pos), 1.0,
		},
	};

}

void mat4_print(mat4 m) {
	printf(
		"mat4(%.8g, %.8g, %.8g, %.8g,\n"
		"     %.8g, %.8g, %.8g, %.8g,\n"
		"     %.8g, %.8g, %.8g, %.8g,\n"
		"     %.8g, %.8g, %.8g, %.8g)\n",
		m.m[0], m.m[1], m.m[2], m.m[3],
		m.m[4], m.m[5], m.m[6], m.m[7],
		m.m[8], m.m[9], m.m[10], m.m[11],
		m.m[12], m.m[13], m.m[14], m.m[15]
	);
}

