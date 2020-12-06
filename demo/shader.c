// wengwengweng

#include <math.h>
#include <dirty/dirty.h>

d_img img;

color spiral(color oc, int x, int y, int w, int h) {

	vec2 uv = vec2f((float)x / (float)w - 0.5, (float)y / (float)h - 0.5);
	float angle = atan2(uv.y, uv.x);
	float dis = vec2_len(uv);
	float c = sin(dis * 48.0 + d_time() * 8 + angle);

	return colori(c * 255, c * 255, c * 255, 255);

}

void init() {
	img = d_make_img(d_width(), d_height());
}

void frame() {

	if (d_key_pressed(D_KEY_ESC)) {
		d_quit();
	}

	d_img_shade(&img, spiral);
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

