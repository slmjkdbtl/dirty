// wengwengweng

#include <stdlib.h>
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

	if (d_mouse_pressed(D_MOUSE_LEFT)) {
		pos = d_mouse_pos();
	}

// 	d_set_blend(D_REPLACE);

	d_clear();
	d_draw_img(&img, pos);
	// TODO: not replacing
	d_draw_rect(vec2f(0, 0), d_mouse_pos(), colori(0, 0, 255, 0));
	d_draw_circle(d_mouse_pos(), 3, colori(0, 255, 255 ,255));
	d_draw_text("oh hi", vec2f(0, 0));
	printf("%f\n", 1.0 / d_dt());

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

