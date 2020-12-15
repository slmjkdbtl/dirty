// wengwengweng

#define DIRTY_IMPL
#include "../dirty.h"

d_sound snd;
d_sound track;
d_playback *track_pb;

void frame() {

	if (d_key_pressed(D_KEY_ESC)) {
		d_quit();
	}

	if (d_key_pressed(D_KEY_SPACE)) {
		track_pb->paused = !track_pb->paused;
	}

	if (d_key_pressed(D_KEY_Q)) {
		d_play(&snd);
	}

	d_clear();

	if (track_pb->paused) {
		d_draw_text("paused", vec2f(0, 0));
	} else {
		d_draw_text("playing", vec2f(0, 0));
	}

}

void init() {
	snd = d_load_sound("res/shoot.dsnd");
	track = d_load_sound("res/yo.dsnd");
	track_pb = d_play(&track);
}

int main() {
	d_run((d_desc) {
		.title = "audio",
		.init = init,
		.frame = frame,
		.width = 240,
		.height = 240,
		.scale = 2,
	});
}

