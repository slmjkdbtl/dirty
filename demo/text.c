// wengwengweng

#include <string.h>
#include <dirty/dirty.h>

char buf[256];

static void frame() {

	if (d_key_pressed(D_KEY_ESC)) {
		d_quit();
	}

	if (d_key_pressed(D_KEY_BACKSPACE)) {
		buf[strlen(buf) - 1] = '\0';
	}

	const char *input = d_tinput();

	if (strlen(input) > 0) {
		strcat(buf, input);
	}

	d_draw_lrect(vec2f(0.0, 0.0), vec2f(320.0, -320.0), 3.0, WHITE);
	d_draw_text(buf, 24.0, 320.0, D_TOP_LEFT, WHITE);

}

int main() {
	d_init("text", 640, 480);
	d_run(frame);
}

