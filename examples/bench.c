// wengwengweng

#include <stdio.h>
#include <stdarg.h>
#include <dirty/dirty.h>

int cnt = 0;

const char* fmt(const char* fmt, ...) {

	static char buf[256];
	va_list args;

	va_start(args, fmt);
	vsnprintf(buf, 256, fmt, args);
	va_end(args);

	return buf;

}

static void frame() {

	if (d_key_pressed(D_KEY_ESC)) {
		d_quit();
	}

	if (d_key_pressed(D_KEY_SPACE)) {
		cnt += 1000;
	}

	for (int i = 0; i < cnt; i++) {
		d_push();
		d_move_xy(vec2_rand(d_coord(D_TOP_LEFT), d_coord(D_BOT_RIGHT)));
		d_draw_text("@", 12.0, D_CENTER, coloru());
		d_pop();
	}

	printf("%s\n", fmt("%d %d", (int)(1.0 / d_dt()), cnt));

// 	d_move_xy(d_coord(D_TOP_LEFT));
// 	d_move_xy(vec2f(16.0, -16.0));
// 	d_draw_rect(vec2f(0.0, 0.0), vec2f(128.0, -28.0), colorf(0.0, 0.0, 0.0, 1.0));
// 	d_move_xy(vec2f(8.0, -8.0));
// 	d_draw_text(fmt("%d %d", (int)(1.0 / d_dt(), cnt)), 12.0, D_TOP_LEFT, coloru());

}

int main() {
	d_init("bench", 640, 480);
	d_run(frame);
}

