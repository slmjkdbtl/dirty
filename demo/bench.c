// wengwengweng

#include <dirty/dirty.h>

int cnt = 0;

void frame() {

	if (d_key_pressed(D_KEY_ESC)) {
		d_quit();
	}

	if (d_key_pressed(D_KEY_SPACE)) {
		cnt += 1000;
	}

	for (int i = 0; i < cnt; i++) {
		d_push();
		d_move_xy(vec2_rand(d_coord(D_TOP_LEFT), d_coord(D_BOT_RIGHT)));
		d_draw_text("@", 12.0, 0.0, D_CENTER, coloru());
		d_pop();
	}

	d_move_xy(d_coord(D_TOP_LEFT));
	d_move_xy(vec2f(16.0, -16.0));
	d_draw_rect(vec2f(0.0, 0.0), vec2f(128.0, -28.0), colorf(0.0, 0.0, 0.0, 1.0));
	d_move_xy(vec2f(8.0, -8.0));
	d_draw_text(d_fmt("%d %d", (int)(1.0 / d_dt()), cnt), 12.0, 0.0, D_TOP_LEFT, coloru());

}

int main() {
	d_run((d_desc) {
		.title = "bench",
		.frame = frame,
	});
}

