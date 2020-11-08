// wengwengweng

#include <dirty/dirty.h>

d_img img;

void frame() {

	if (d_key_pressed(D_KEY_ESC)) {
		d_quit();
	}

	d_draw_img(&img, quadu(), WHITE);

}

void init() {
	img = d_load_img("res/acid2.png");
}

int main() {
	d_run((d_desc) {
		.title = "sprite",
		.init = init,
		.frame = frame,
	});
}

