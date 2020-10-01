// wengwengweng

#include <string.h>
#include <dirty/dirty.h>

#define BUF_SIZE 256

char str[BUF_SIZE];

void frame() {

	if (d_key_pressed(D_KEY_ESC)) {
		d_quit();
	}

	if (d_key_pressed(D_KEY_BACKSPACE)) {
		if (d_key_mod(D_KMOD_ALT)) {
			memset(str, 0, sizeof(str));
		} else {
			str[strlen(str) - 1] = '\0';
		}
	}

	char input = d_input();

	if (input && strlen(str) < BUF_SIZE) {
		char tmpstr[2];
		tmpstr[0] = input;
		tmpstr[1] = '\0';
		strcat(str, tmpstr);
	}

	d_move_xy(vec2f(-120.0, 120.0));
	d_draw_lrect(vec2f(0.0, 0.0), vec2f(320.0, -320.0), 3.0, WHITE);
	d_ftext ftext = d_fmt_text(str, 24.0, 320.0, D_TOP_LEFT, WHITE);
	vec2 cpos = d_ftext_cpos(&ftext, strlen(str));
	cpos.x += 4.0;
	d_draw_ftext(&ftext);
	d_draw_line(cpos, vec2f(cpos.x, cpos.y - 24.0), 5.0, WHITE);

}

int main() {
	d_run((d_desc) {
		.title = "text",
		.frame = frame,
	});
}

