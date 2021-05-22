// wengwengweng

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

#define D_IMPL
#define D_CPU
#include <d_plat.h>
#include <d_fs.h>
#include <d_math.h>
#include <d_app.h>
#include <d_gfx.h>

#define WIDTH 64
#define HEIGHT 64
#define SCALE 10

d_img canvas;

void init() {

	d_gfx_init((d_gfx_desc) {
		.width = WIDTH,
		.height = HEIGHT,
		.clear_color = colori(0, 0, 0, 255),
	});

	canvas = d_make_img(WIDTH, HEIGHT);

}

void frame() {

	if (d_app_key_pressed(D_KEY_ESC)) {
		d_app_quit();
	}

	if (d_app_key_pressed(D_KEY_F)) {
		d_app_set_fullscreen(!d_app_fullscreen());
	}

	d_gfx_drawon(&canvas);

	if (d_app_mouse_down(D_MOUSE_LEFT)) {
		vec2 mpos = d_gfx_mouse_pos();
		vec2 mdpos = d_gfx_mouse_dpos();
		d_blit_line(vec2_sub(mpos, mdpos), mpos, colorx(0x000000ff));
	}

	d_gfx_drawon(NULL);

	d_gfx_clear();
	d_blit_bg();
	d_blit_img(&canvas, vec2f(0, 0));
	d_gfx_present();

}

int main() {
	d_app_run((d_app_desc) {
		.title = "3d",
		.init = init,
		.frame = frame,
		.width = WIDTH * SCALE,
		.height = HEIGHT * SCALE,
	});
}
