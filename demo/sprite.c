// wengwengweng

#include <stdlib.h>
#include <dirty/dirty.h>

int main() {

	d_init("tri", 640, 480);

	d_tex tex = d_load_tex("res/acid2.png");

	while (d_running()) {

		d_clear();

		if (d_key_pressed(D_KEY_ESC)) {
			d_quit();
		}

		d_draw_tex(&tex, quadu(), WHITE);
		d_frame();

	}

}

