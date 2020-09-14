// wengwengweng

#include <dirty/dirty.h>

d_mesh cube;

static void frame() {

	if (d_key_pressed(D_KEY_ESC)) {
		d_quit();
	}

	d_rot_x(radf(-45.0));
	d_rot_y(radf(-45.0));
	d_scale(vec3f(90.0, 90.0, 90.0));
	d_draw_mesh(&cube);

}

int main() {

	d_init("cube", 640, 480);

	d_vertex verts[] = {
		{
			.pos = vec3f(1, 1, 1),
			.normal = vec3f(0, 0, 0),
			.uv = vec2f(0, 0),
			.color = colorf(1, 1, 1, 1),
		},
		{
			.pos = vec3f(-1, 1, 1),
			.normal = vec3f(0, 0, 0),
			.uv = vec2f(0, 0),
			.color = colorf(0, 1, 1, 1),
		},
		{
			.pos = vec3f(-1, -1, 1),
			.normal = vec3f(0, 0, 0),
			.uv = vec2f(0, 0),
			.color = colorf(0, 0, 1, 1),
		},
		{
			.pos = vec3f(1, -1, 1),
			.normal = vec3f(0, 0, 0),
			.uv = vec2f(0, 0),
			.color = colorf(1, 0, 1, 1),
		},
		{
			.pos = vec3f(1, 1, -1),
			.normal = vec3f(0, 0, 0),
			.uv = vec2f(0, 0),
			.color = colorf(1, 1, 0, 1),
		},
		{
			.pos = vec3f(-1, 1, -1),
			.normal = vec3f(0, 0, 0),
			.uv = vec2f(0, 0),
			.color = colorf(0, 1, 0, 1),
		},
		{
			.pos = vec3f(-1, -1, -1),
			.normal = vec3f(0, 0, 0),
			.uv = vec2f(0, 0),
			.color = colorf(0, 0, 0, 1),
		},
		{
			.pos = vec3f(1, -1, -1),
			.normal = vec3f(0, 0, 0),
			.uv = vec2f(0, 0),
			.color = colorf(1, 0, 0, 1),
		},
	};

	d_index indices[] = {
		0, 1, 2,
		0, 2, 3,
		4, 0, 3,
		4, 3, 7,
		4, 5, 1,
		4, 1, 0,
		5, 4, 7,
		5, 7, 6,
		1, 5, 6,
		1, 6, 2,
		3, 2, 6,
		3, 6, 7,
	};

	cube = d_make_mesh(verts, 8, indices, 36);

	d_run(frame);

}

