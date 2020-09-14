// wengwengweng

#include <string.h>
#include <dirty/dirty.h>

char buf[256];

static void frame() {

	if (d_key_pressed(D_KEY_ESC)) {
		d_quit();
	}

	if (d_key_pressed(D_KEY_BACKSPACE)) {
		if (d_key_has_mod(D_KMOD_ALT)) {
			memset(buf, 0, sizeof(buf));
		} else {
			buf[strlen(buf) - 1] = '\0';
		}
	}

	const char *input = d_tinput();

	if (strlen(input) > 0) {
		strcat(buf, input);
	}

	d_move_xy(vec2f(-120.0, 120.0));
	d_draw_lrect(vec2f(0.0, 0.0), vec2f(320.0, -320.0), 3.0, WHITE);
	d_ftext ftext = d_fmt_text(buf, 24.0, 320.0, D_TOP_LEFT, WHITE);
	vec2 cpos = d_ftext_cpos(&ftext, strlen(buf));
	cpos.x += 4.0;
	d_draw_ftext(&ftext);
	d_draw_line(cpos, vec2f(cpos.x, cpos.y - 24.0), 5.0, WHITE);

}

int main() {
	d_init("text", 640, 480);
	d_run(frame);
}

