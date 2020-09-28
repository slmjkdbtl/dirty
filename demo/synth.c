// wengwengweng

#include <dirty/dirty.h>

static void frame() {

	if (d_key_pressed(D_KEY_ESC)) {
		d_quit();
	}

	int map[128] = {
		[D_KEY_Q] = 59,
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
		[D_KEY_O] = 73,
		[D_KEY_L] = 74,
	};

	for (int i = 0; i < 128; i++) {
		if (map[i]) {
			if (d_key_pressed(i)) {
				d_synth_play(map[i]);
			}
			if (d_key_released(i)) {
				d_synth_release(map[i]);
			}
		}
	}

}

void init() {

	d_synth_wav(d_wav_saw);

	d_envelope *e = d_synth_envelope();

	e->attack = 0.0;
	e->decay = 0.0;
	e->sustain = 1.0;
	e->release = 0.0;

}

int main() {
	d_run((d_desc) {
		.title = "synth",
		.init = init,
		.frame = frame,
	});
}

