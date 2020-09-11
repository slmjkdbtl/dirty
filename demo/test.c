// wengwengweng

#include <stdlib.h>
#include <dirty/dirty.h>

d_mesh tri;
d_tex tex;
d_canvas canvas;

d_vertex verts[3];
d_index indices[3];

static void frame() {

	if (d_key_pressed(D_KEY_F)) {
		d_set_fullscreen(!d_fullscreen());
	}

	if (d_key_pressed(D_KEY_ESC)) {
		d_quit();
	}

	d_draw_raw(verts, 3, indices, 3, NULL);
	d_draw_lrect(vec2f(-200.0, -200.0), vec2f(200.0, 200.0), 5.0, coloru());
	d_draw_text("hihi", 64.0, D_CENTER, coloru());

}

int main() {

	d_init("test", 640, 480);

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

	tex = d_load_tex("res/acid2.png");
	canvas = d_make_canvas(320, 320);
	tri = d_make_mesh(verts, 3, indices, 3);

	d_fmt_text("1234567890", 32.0, D_TOP_LEFT, 72.0, WHITE);

	d_use_canvas(&canvas);
	d_draw_mesh(&tri);
	d_use_canvas(NULL);

	d_run(frame);

}

