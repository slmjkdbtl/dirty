#define D_IMPL
#include <d_plat.h>
#include <d_math.h>
#include <d_app.h>
#include <d_gfx.h>

d_img img;

void init(void) {
	d_gfx_init((d_gfx_desc) {
		.width = 240,
		.height = 240,
		.clear_color = d_colori(0, 0, 0, 255),
	});
	img = d_img_new(d_gfx_width(), d_gfx_height());
}

void frame(void) {

	if (d_app_key_pressed(D_KEY_ESC)) {
		d_app_quit();
	}

	d_vec2 mpos = d_gfx_mouse_pos();

	for (int x = 0; x < img.width; x++) {
		for (int y = 0; y < img.height; y++) {
			d_vec2 uv = d_vec2f(
				(float)(x - mpos.x) / (float)img.width,
				(float)(y - mpos.y) / (float)img.height
			);
			float angle = atan2(uv.y, uv.x);
			float dis = d_vec2_len(uv);
			float c = sin(dis * 48.0 + d_app_time() * 8 + angle);
			d_img_set(&img, x, y, d_colori(c * 255, c * 255, c * 255, 255));
		}
	}

	d_blit_img(&img, d_vec2f(0, 0));
	d_gfx_present();

}

int main(void) {
	d_app_run((d_app_desc) {
		.title = "shader",
		.init = init,
		.frame = frame,
		.width = 240,
		.height = 240,
	});
}

