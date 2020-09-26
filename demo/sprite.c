// wengwengweng

#include <dirty/dirty.h>

d_tex tex;

static void frame() {

	if (d_key_pressed(D_KEY_ESC)) {
		d_quit();
	}

	d_draw_tex(&tex, quadu(), WHITE);

}

int main() {

	d_init("sprite", 640, 480);

	tex = d_load_tex("res/acid2.png");

	d_sprite_data data = d_load_ase("res/test.json");

	for (int i = 0; i < data.frame_n; i++) {
		printf("%f\n", data.frames[i].x);
	}

	d_run(frame);

}

