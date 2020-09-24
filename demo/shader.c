// wengwengweng

#include <stdlib.h>
#include <dirty/dirty.h>

d_shader shader;

static void frame() {

	if (d_key_pressed(D_KEY_ESC)) {
		d_quit();
	}

	d_use_shader(&shader);
	d_send_f("u_time", d_time());
	d_draw_rect(d_coord(D_TOP_LEFT), d_coord(D_BOT_RIGHT), WHITE);
	d_use_shader(NULL);

}

int main() {

	d_init("shader", 640, 480);
	shader = d_load_shader(NULL, "res/spiral.frag");
	d_run(frame);

}

