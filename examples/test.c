// wengwengweng

#include <dirty/dirty.h>

static void frame() {

	if (d_key_pressed(D_KEY_F)) {
		d_set_fullscreen(!d_fullscreen());
	}

	if (d_key_pressed(D_KEY_ESC)) {
		d_quit(EXIT_SUCCESS);
	}

}

int main() {

	d_init("test", 640, 480);
	d_run(frame);

}

