// wengwengweng

#include <stdlib.h>
#include <dirty/dirty.h>

d_mesh tri_mesh;
d_tex tex;
d_sound snd;
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

	if (d_key_pressed(D_KEY_SPACE)) {
		d_sound_play(&snd);
	}

// 	d_scale_xy(vec2f(2.0, 2.0));
	d_push();
	d_move_xy(d_mouse_pos());
// 	d_draw_raw(verts, 3, indices, 3);
	d_draw_tex(&tex, quadu());
	d_draw_text("hihi", 120.0);
	d_pop();
// 	d_draw_rect(vec2f(12.0, 12.0), vec2f(-12.0, -12.0));
	d_draw_line(vec2f(-100.0, -100.0), vec2f(100.0, 100.0), 3.0);
// 	d_draw_canvas(&canvas);
// 	d_draw_mesh(&tri_mesh);

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

	tri_mesh = d_make_mesh(verts, 3, indices, 3);
	tex = d_load_tex("res/acid2.png");
	canvas = d_make_canvas(d_width(), d_height());
// 	canvas = d_make_canvas(320, 120);
	snd = d_load_sound("res/shoot.ogg");

	d_use_canvas(&canvas);
	d_draw_mesh(&tri_mesh);
	d_use_canvas(NULL);

	d_run(frame);

}

