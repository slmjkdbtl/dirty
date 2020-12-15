// wengwengweng

#include <math.h>
#define D_CPU
#define DIRTY_IMPL
#include "../dirty.h"

d_img img;

void init() {
	img = d_make_img(d_width(), d_height());
}

void frame() {

	if (d_key_pressed(D_KEY_ESC)) {
		d_quit();
	}

	for (int x = 0; x < img.width; x++) {
		for (int y = 0; y < img.height; y++) {
			vec2 uv = vec2f((float)x / (float)img.width - 0.5, (float)y / (float)img.height - 0.5);
			float angle = atan2(uv.y, uv.x);
			float dis = vec2_len(uv);
			float c = sin(dis * 48.0 + d_time() * 8 + angle);
			d_img_set(&img, x, y, colori(c * 255, c * 255, c * 255, 255));
		}
	}

	d_draw_img(&img, vec2f(0, 0));

}

int main() {
	d_run((d_desc) {
		.title = "shader",
		.init = init,
		.frame = frame,
		.width = 240,
		.height = 240,
		.scale = 2,
	});
}

