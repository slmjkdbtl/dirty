// wengwengweng

#include <dirty/dirty.h>

d_mesh tri_mesh;

static void frame() {

	if (d_key_pressed(D_KEY_F)) {
		d_set_fullscreen(!d_fullscreen());
	}

	if (d_key_pressed(D_KEY_ESC)) {
		d_quit(EXIT_SUCCESS);
	}

	d_draw(&tri_mesh);

}

int main() {

	d_init("test", 640, 480);

	d_vertex verts[] = {
		{
			.pos = vec3f(0.0, 120.0, 0.0),
			.normal = vec3f(0.0, 0.0, 1.0),
			.uv = vec2f(0.0, 0.0),
			.color = colorf(1.0, 0.0, 0.0, 1.0)
		},
		{
			.pos = vec3f(-160.0, -120.0, 0.0),
			.normal = vec3f(0.0, 0.0, 1.0),
			.uv = vec2f(0.0, 0.0),
			.color = colorf(0.0, 1.0, 0.0, 1.0)
		},
		{
			.pos = vec3f(160.0, -120.0, 0.0),
			.normal = vec3f(0.0, 0.0, 1.0),
			.uv = vec2f(0.0, 0.0),
			.color = colorf(0.0, 0.0, 1.0, 1.0)
		},
	};

	unsigned int indices[] = {
		0, 1, 2,
	};

	tri_mesh = d_make_mesh(verts, sizeof(verts), indices, sizeof(indices));

	d_run(frame);

}

