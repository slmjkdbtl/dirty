// wengwengweng

#include <dirty/dirty.h>

static void frame() {

	if (d_key_pressed(D_KEY_ESC)) {
		d_quit();
	}

	if (d_mouse_down(D_MOUSE_LEFT)) {
		printf("%f\n", d_time());
	}

}

int main() {

	d_init("test", 640, 480);
	d_vsync(true);
	d_run(frame);

}

