// wengwengweng

#include <dirty/dirty.h>

d_vertex verts[3];
d_index indices[3];

static void frame() {

	if (d_key_pressed(D_KEY_ESC)) {
		d_quit();
	}

	d_draw_raw(verts, 3, indices, 3);

}

int main() {

	d_init("tri", 640, 480);

	verts[0] = (d_vertex) {
		.pos = vec3f(0.0, 120.0, 0.0),
		.normal = vec3f(0.0, 0.0, 1.0),
		.uv = vec2f(0.0, 0.0),
		.color = colorf(1.0, 0.0, 0.0, 1.0)
	};

	verts[1] = (d_vertex) {
		.pos = vec3f(-160.0, -120.0, 0.0),
		.normal = vec3f(0.0, 0.0, 1.0),
		.uv = vec2f(0.0, 0.0),
		.color = colorf(0.0, 1.0, 0.0, 1.0)
	};

	verts[2] = (d_vertex) {
		.pos = vec3f(160.0, -120.0, 0.0),
		.normal = vec3f(0.0, 0.0, 1.0),
		.uv = vec2f(0.0, 0.0),
		.color = colorf(0.0, 0.0, 1.0, 1.0)
	};

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	d_run(frame);

}

