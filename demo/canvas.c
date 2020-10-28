// wengwengweng

#include <dirty/dirty.h>

d_canvas canvas;

void init() {
	canvas = d_make_canvas(d_width(), d_height());
}

void frame() {

	if (d_key_pressed(D_KEY_ESC)) {
		d_quit();
	}

	if (d_key_pressed(D_KEY_SPACE)) {
		d_img i = d_canvas_capture(&canvas);
		d_img_save(&i, "test.png");
		d_free_img(&i);
	}

	d_use_canvas(&canvas);
	d_draw_text("oh hi", 12.0, 0.0, D_CENTER, WHITE);
	d_use_canvas(NULL);
	d_draw_canvas(&canvas, WHITE);

}

int main() {
	d_run((d_desc) {
		.title = "canvas",
		.init = init,
		.frame = frame,
	});
}

