// wengwengweng

#include <stdlib.h>
#include <dirty/dirty.h>

d_tex tex;

static void frame() {

	if (d_key_pressed(D_KEY_ESC)) {
		d_quit();
	}

	d_draw_tex(&tex, quadu(), WHITE);

}

int main() {

	d_init("tri", 640, 480);

	tex = d_load_tex("res/acid2.png");

	d_run(frame);

}

