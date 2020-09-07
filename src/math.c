// wengwengweng

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <dirty/dirty.h>

vec2 vec2f(float x, float y) {
	return (vec2) {
		.x = x,
		.y = y,
	};
}

vec2 vec2u() {
	return vec2f(0.0, 0.0);
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

vec2 vec2_mult(vec2 p1, vec2 p2) {
	return (vec2) {
		.x = p1.x * p2.x,
		.y = p1.y * p2.y,
	};
}

float vec2_dist(vec2 p1, vec2 p2) {
	return sqrt(
		(p1.x - p2.x) * (p1.x - p2.x) +
		(p1.y - p2.y) * (p1.y - p2.y)
	);
}

float vec2_len(vec2 p) {
	return vec2_dist(p, vec2f(0.0, 0.0));
}

vec2 vec2_unit(vec2 p) {
	return vec2_scale(p, 1.0 / vec2_len(p));
}

vec3 vec2_cross(vec2 p1, vec2 p2) {
	return vec3_cross(vec3f(p1.x, p1.y, 0.0), vec3f(p2.x, p2.y, 0.0));
}

float vec2_dot(vec2 p1, vec2 p2) {
	return p1.x * p2.x + p1.y * p2.y;
}

vec2 vec2_normal(vec2 p) {
	return vec2f(p.y, -p.x);
}

float vec2_angle(vec2 p1, vec2 p2) {
	return atan2(p1.y - p2.y, p1.x - p2.x);
}

vec2 vec2_min(vec2 a, vec2 b) {
	return (vec2) {
		.x = fminf(a.x, b.x),
		.y = fminf(a.y, b.y),
	};
}

vec2 vec2_max(vec2 a, vec2 b) {
	return (vec2) {
		.x = fmaxf(a.x, b.x),
		.y = fmaxf(a.y, b.y),
	};
}

vec2 vec2_lerp(vec2 from, vec2 to, float t) {
	return (vec2) {
		.x = lerpf(from.x, to.x, t),
		.y = lerpf(from.y, to.y, t),
	};
}

vec2 vec2_rand(vec2 p1, vec2 p2) {
	return (vec2) {
		.x = randfi(p1.x, p2.x),
		.y = randfi(p1.y, p2.y),
	};
}

vec2 vec2_clamp(vec2 p, vec2 low, vec2 hi) {
	return (vec2) {
		.x = clampf(p.x, low.x, hi.x),
		.y = clampf(p.x, low.y, hi.y),
	};
}

bool vec2_eq(vec2 p1, vec2 p2) {
	return p1.x == p2.x && p1.y == p2.y;
}

void vec2_print(vec2 p) {
	printf("vec2(%.8g, %.8g)\n", p.x, p.y);
}

vec3 vec3f(float x, float y, float z) {
	return (vec3) {
		.x = x,
		.y = y,
		.z = z,
	};
}

vec3 vec3u() {
	return vec3f(0.0, 0.0, 0.0);
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

vec3 vec3_mult(vec3 p1, vec3 p2) {
	return (vec3) {
		.x = p1.x * p2.x,
		.y = p1.y * p2.y,
		.z = p1.z * p2.z,
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
	return vec3_dist(p, vec3f(0.0, 0.0, 0.0));
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

vec3 vec3_min(vec3 a, vec3 b) {
	return (vec3) {
		.x = fminf(a.x, b.x),
		.y = fminf(a.y, b.y),
		.z = fminf(a.z, b.z),
	};
}

vec3 vec3_max(vec3 a, vec3 b) {
	return (vec3) {
		.x = fmaxf(a.x, b.x),
		.y = fmaxf(a.y, b.y),
		.z = fmaxf(a.z, b.z),
	};
}

vec3 vec3_lerp(vec3 from, vec3 to, float t) {
	return (vec3) {
		.x = lerpf(from.x, to.x, t),
		.y = lerpf(from.y, to.y, t),
		.z = lerpf(from.z, to.z, t),
	};
}

vec3 vec3_rand(vec3 p1, vec3 p2) {
	return (vec3) {
		.x = randfi(p1.x, p2.x),
		.y = randfi(p1.y, p2.y),
		.z = randfi(p1.z, p2.z),
	};
}

vec3 vec3_clamp(vec3 p, vec3 low, vec3 hi) {
	return (vec3) {
		.x = clampf(p.x, low.x, hi.x),
		.y = clampf(p.x, low.y, hi.y),
		.z = clampf(p.x, low.z, hi.z),
	};
}

bool vec3_eq(vec3 p1, vec3 p2) {
	return p1.x == p2.x && p1.y == p2.y && p1.z == p2.z;
}

void vec3_print(vec3 p) {
	printf("vec3(%.8g, %.8g, %.8g)\n", p.x, p.y, p.z);
}

vec4 vec4f(float x, float y, float z, float w) {
	return (vec4) {
		.x = x,
		.y = y,
		.z = z,
		.w = w,
	};
}

vec4 vec4u() {
	return vec4f(0.0, 0.0, 0.0, 1.0);
}

color colorf(float r, float g, float b, float a) {
	return (color) {
		.r = r,
		.g = g,
		.b = b,
		.a = a,
	};
}

color colori(int hex, float a) {
	return (color) {
		.r = ((hex >> 16) & 0xff) / 255.0,
		.g = ((hex >> 8) & 0xff) / 255.0,
		.b = ((hex) & 0xff) / 255.0,
		.a = a,
	};
}

color coloru() {
	return colorf(1.0, 1.0, 1.0, 1.0);
}

color color_invert(color c) {
	return (color) {
		.r = 1.0 - c.r,
		.g = 1.0 - c.g,
		.b = 1.0 - c.b,
		.a = c.a,
	};
}

color color_darken(color c, float a) {
	return (color) {
		.r = c.r - a,
		.g = c.g - a,
		.b = c.b - a,
		.a = c.a,
	};
}

bool color_eq(color c1, color c2) {
	return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b && c1.a == c2.a;
}

void color_print(color c) {
	printf("color(%.8g, %.8g, %.8g, %.8g)\n", c.r, c.g, c.b, c.a);
}

mat4 mat4f(float m[16]) {
	return (mat4) {
		.m = {
			m[0], m[1], m[2], m[3],
			m[4], m[5], m[6], m[7],
			m[8], m[9], m[10], m[11],
			m[12], m[13], m[14], m[15]
		},
	};
}

mat4 mat4u() {
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

	mat4 out = mat4u();

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			out.m[i * 4 + j] =
				m1.m[0 * 4 + j] * m2.m[i * 4 + 0] +
				m1.m[1 * 4 + j] * m2.m[i * 4 + 1] +
				m1.m[2 * 4 + j] * m2.m[i * 4 + 2] +
				m1.m[3 * 4 + j] * m2.m[i * 4 + 3];
		}
	}

	return out;

}

vec4 mat4_mult_vec4(mat4 m, vec4 p) {
	return (vec4) {
		.x = p.x * m.m[0] + p.y * m.m[4] + p.z * m.m[8] + p.w * m.m[12],
		.y = p.x * m.m[1] + p.y * m.m[5] + p.z * m.m[9] + p.w * m.m[13],
		.z = p.x * m.m[2] + p.y * m.m[6] + p.z * m.m[10] + p.w * m.m[14],
		.w = p.x * m.m[3] + p.y * m.m[7] + p.z * m.m[11] + p.w * m.m[15]
	};
}

vec3 mat4_mult_vec3(mat4 m, vec3 p) {
	vec4 p4 = mat4_mult_vec4(m, vec4f(p.x, p.y, p.z, 1.0));
	return vec3f(p4.x, p4.y, p4.z);
}

vec2 mat4_mult_vec2(mat4 m, vec2 p) {
	vec3 p3 = mat4_mult_vec3(m, vec3f(p.x, p.y, 0.0));
	return vec2f(p3.x, p3.y);
}

mat4 mat4_invert(mat4 m) {

	mat4 out = mat4u();

	float f00 = m.m[10] * m.m[15] - m.m[14] * m.m[11];
	float f01 = m.m[9] * m.m[15] - m.m[13] * m.m[11];
	float f02 = m.m[9] * m.m[14] - m.m[13] * m.m[10];
	float f03 = m.m[8] * m.m[15] - m.m[12] * m.m[11];
	float f04 = m.m[8] * m.m[14] - m.m[12] * m.m[10];
	float f05 = m.m[8] * m.m[13] - m.m[12] * m.m[9];
	float f06 = m.m[6] * m.m[15] - m.m[14] * m.m[7];
	float f07 = m.m[5] * m.m[15] - m.m[13] * m.m[7];
	float f08 = m.m[5] * m.m[14] - m.m[13] * m.m[6];
	float f09 = m.m[4] * m.m[15] - m.m[12] * m.m[7];
	float f10 = m.m[4] * m.m[14] - m.m[12] * m.m[6];
	float f11 = m.m[5] * m.m[15] - m.m[13] * m.m[7];
	float f12 = m.m[4] * m.m[13] - m.m[12] * m.m[5];
	float f13 = m.m[6] * m.m[11] - m.m[10] * m.m[7];
	float f14 = m.m[5] * m.m[11] - m.m[9] * m.m[7];
	float f15 = m.m[5] * m.m[10] - m.m[9] * m.m[6];
	float f16 = m.m[4] * m.m[11] - m.m[8] * m.m[7];
	float f17 = m.m[4] * m.m[10] - m.m[8] * m.m[6];
	float f18 = m.m[4] * m.m[9] - m.m[8] * m.m[5];

	out.m[0] = m.m[5] * f00 - m.m[6] * f01 + m.m[7] * f02;
	out.m[4] = -(m.m[4] * f00 - m.m[6] * f03 + m.m[7] * f04);
	out.m[8] = m.m[4] * f01 - m.m[5] * f03 + m.m[7] * f05;
	out.m[12] = -(m.m[4] * f02 - m.m[5] * f04 + m.m[6] * f05);

	out.m[1] = -(m.m[1] * f00 - m.m[2] * f01 + m.m[3] * f02);
	out.m[5] = m.m[0] * f00 - m.m[2] * f03 + m.m[3] * f04;
	out.m[9] = -(m.m[0] * f01 - m.m[1] * f03 + m.m[3] * f05);
	out.m[13] = m.m[0] * f02 - m.m[1] * f04 + m.m[2] * f05;

	out.m[2] = m.m[1] * f06 - m.m[2] * f07 + m.m[3] * f08;
	out.m[6] = -(m.m[0] * f06 - m.m[2] * f09 + m.m[3] * f10);
	out.m[10] = m.m[0] * f11 - m.m[1] * f09 + m.m[3] * f12;
	out.m[14] = -(m.m[0] * f08 - m.m[1] * f10 + m.m[2] * f12);

	out.m[3] = -(m.m[1] * f13 - m.m[2] * f14 + m.m[3] * f15);
	out.m[7] = m.m[0] * f13 - m.m[2] * f16 + m.m[3] * f17;
	out.m[11] = -(m.m[0] * f14 - m.m[1] * f16 + m.m[3] * f18);
	out.m[15] = m.m[0] * f15 - m.m[1] * f17 + m.m[2] * f18;

	float det =
		m.m[0] * out.m[0] +
		m.m[1] * out.m[4] +
		m.m[2] * out.m[8] +
		m.m[3] * out.m[12];

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			out.m[i * 4 + j] *= (1.0 / det);
		}
	}

	return out;

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

quat quatf(float x, float y, float z, float w) {
	return (quat) {
		.x = x,
		.y = y,
		.z = z,
		.w = w,
	};
}

quat quatu() {
	return quatf(0.0, 0.0, 0.0, 1.0);
}

quad quadf(float x, float y, float w, float h) {
	return (quad) {
		.x = x,
		.y = y,
		.w = w,
		.h = h,
	};
}

quad quadu() {
	return quadf(0.0, 0.0, 1.0, 1.0);
}

float degf(float r) {
    return r * (180.0 / M_PI);
}

float radf(float d) {
    return d / (180.0 / M_PI);
}

float clampf(float v, float low, float hi) {
	return fmaxf(low, fmin(v, hi));
}

float lerpf(float a, float b, float t) {
    return a + (b - a) * t;
}

float randf() {
	return (float)rand() / (float)RAND_MAX;
}

float randfi(float low, float hi) {
	return low + randf() * (hi - low);
}

bool pt_rect(vec2 pt, vec2 p1, vec2 p2) {
	vec2 pp1 = vec2_min(p1, p2);
	vec2 pp2 = vec2_max(p1, p2);
	return pt.x >= pp1.x && pt.x <= pp2.x && pt.y >= pp1.y && pt.y <= pp2.y;
}

