// wengwengweng

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirty/dirty.h>

int main() {

	d_init("test", 640, 480);

	d_canvas canvas = d_make_canvas(320, 320);

	d_use_canvas(&canvas);
	d_draw_text("hihi", 64.0, 0.0, D_CENTER, coloru());
	d_use_canvas(NULL);

	while (d_running()) {

		d_clear();

		if (d_key_pressed(D_KEY_ESC)) {
			d_quit();
		}

		d_draw_canvas(&canvas, WHITE);
		d_frame();

	}


}

