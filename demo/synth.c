// wengwengweng

#include <stdlib.h>
#include <dirty/dirty.h>

d_tex tex;

static void frame() {

	if (d_key_pressed(D_KEY_ESC)) {
		d_quit();
	}

	int map[128] = {
		[D_KEY_A] = 60,
		[D_KEY_W] = 61,
		[D_KEY_S] = 62,
		[D_KEY_E] = 63,
		[D_KEY_D] = 64,
		[D_KEY_F] = 65,
		[D_KEY_T] = 66,
		[D_KEY_G] = 67,
		[D_KEY_Y] = 68,
		[D_KEY_H] = 69,
		[D_KEY_U] = 70,
		[D_KEY_J] = 71,
		[D_KEY_K] = 72,
	};

	for (int i = 0; i < 128; i++) {
		if (map[i]) {
			if (d_key_pressed(i)) {
				d_synth_play(map[i]);
			}
		}
	}

}

int main() {

	d_init("synth", 640, 480);
	d_run(frame);

}
