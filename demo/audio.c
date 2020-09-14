// wengwengweng

#include <dirty/dirty.h>

int main() {

	d_init("audio", 640, 480);

	d_sound snd = d_load_sound("res/shoot.ogg");
	d_sound track = d_load_sound("res/yo.ogg");

	d_sound_pb *track_pb = d_play(&track);

	while (d_running()) {

		d_clear();

		if (d_key_pressed(D_KEY_ESC)) {
			d_quit();
		}

		if (d_key_pressed(D_KEY_SPACE)) {
			track_pb->paused = !track_pb->paused;
		}

		if (d_key_pressed(D_KEY_Q)) {
			d_play(&snd);
		}

		if (track_pb->paused) {
			d_draw_text("paused", 24.0, 0.0, D_CENTER, coloru());
		} else {
			d_draw_text("playing", 24.0, 0.0, D_CENTER, coloru());
		}

		d_frame();

	}

}

