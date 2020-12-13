// wengwengweng

#include <stdio.h>
#include <dirty/dirty.h>

d_img img;
vec2 pos;

void init() {
	img = d_load_img("res/wizard.png");
}

void frame() {

	if (d_key_pressed(D_KEY_ESC)) {
		d_quit();
	}

	if (d_key_pressed(D_KEY_F)) {
		d_set_fullscreen(!d_fullscreen());
	}

	if (d_mouse_pressed(D_MOUSE_LEFT)) {
		pos = d_mouse_pos();
	}

	d_clear();

// 	for (int i = 0; i < 1000; i++) {
		d_draw_img(&img, pos);
// 	}
	d_draw_rect(vec2f(0, 0), d_mouse_pos(), colori(0, 0, 255, 100));
	d_draw_circle(d_mouse_pos(), 3, colori(0, 255, 255 ,255));
	d_draw_text("oh hi", vec2f(0, 0));

	d_set_title(d_fmt("%d", d_fps()));

}

int main() {
	d_run((d_desc) {
		.title = "test",
		.init = init,
		.frame = frame,
		.width = 240,
		.height = 240,
		.scale = 2,
	});
}

