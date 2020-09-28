// wengwengweng

#include <dirty/dirty.h>

void frame() {

	if (d_key_pressed(D_KEY_ESC)) {
		d_quit();
	}

	vec2 p1 = vec2f(100.0, 200.0);
	vec2 p2 = vec2f(200.0, 100.0);
	vec2 p3 = d_mouse_pos();
	vec2 p4 = vec2_add(p3, vec2f(24.0, 24.0));

	d_draw_line(p3, vec2f(0.0, 0.0), 3.0, WHITE);

	if (line_rect(line2f(p3, vec2f(0.0, 0.0)), rectf(p1, p2))) {
		d_draw_rect(p1, p2, BLUE);
	} else {
		d_draw_rect(p1, p2, WHITE);
	}

}

int main() {
	d_run((d_desc) {
		.title = "geom",
		.frame = frame,
	});
}

