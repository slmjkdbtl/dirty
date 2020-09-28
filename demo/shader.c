// wengwengweng

#include <stdlib.h>
#include <dirty/dirty.h>

d_shader shader;

void frame() {

	if (d_key_pressed(D_KEY_ESC)) {
		d_quit();
	}

	d_use_shader(&shader);
	d_send_f("u_time", d_time());
	d_draw_rect(d_coord(D_TOP_LEFT), d_coord(D_BOT_RIGHT), WHITE);
	d_use_shader(NULL);

}

void init() {
	shader = d_load_shader(NULL, "res/spiral.frag");
}

int main() {
	d_run((d_desc) {
		.title = "shader",
		.init = init,
		.frame = frame,
	});
}

