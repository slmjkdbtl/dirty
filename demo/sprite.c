#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define D_IMPL
#include <d_fs.h>
#include <d_math.h>
#include <d_app.h>
#include <d_gfx.h>

#define WIDTH 480
#define HEIGHT 480
#define SCALE 2

d_img img;
d_vec2 pos;

void init(void) {
	d_gfx_init((d_gfx_desc) {
		.scale = SCALE,
	});
	img = d_img_load(d_res_path("res/wizard.png"));
}

void frame(void) {

	d_vec2 mpos = d_gfx_mouse_pos();

	if (d_app_key_pressed(D_KEY_ESC)) {
		d_app_quit();
	}

	if (d_app_key_pressed(D_KEY_F)) {
		d_app_set_fullscreen(!d_app_is_fullscreen());
	}

	if (d_app_mouse_pressed(D_MOUSE_LEFT)) {
		pos = d_gfx_mouse_pos();
	}

	d_gfx_clear();
	d_blit_bg();
	d_blit_img(&img, pos);
	d_gfx_present();

}

int main(void) {
	d_app_run((d_app_desc) {
		.title = "sprite",
		.init = init,
		.frame = frame,
		.width = WIDTH,
		.height = HEIGHT,
	});
}
