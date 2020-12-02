// wengwengweng

#include <stdlib.h>
#include <dirty/dirty.h>

d_img img;

void init() {
	img = d_load_img("res/acid2.png");
}

void frame() {

	if (d_key_pressed(D_KEY_ESC)) {
		d_quit();
	}

	d_clear();
// 	d_draw_rect(vec2f(100, 100), d_mouse_pos(), colori(255, 255, 255, 255));
	d_draw_img(&img, vec2f(0, 0));

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

