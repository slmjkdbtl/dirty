#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define D_IMPL
#include <d_plat.h>
#include <d_fs.h>
#include <d_math.h>
#include <d_app.h>
#include <d_gfx.h>

#define WIDTH 240
#define HEIGHT 240
#define SCALE 2

#define FMT_MAX 256

static char* fmt(char *fmt, ...) {

	static char buf[FMT_MAX];
	va_list args;

	va_start(args, fmt);
	vsnprintf(buf, FMT_MAX, fmt, args);
	va_end(args);

	return buf;

}

d_img img;
vec2 pos;

void init(void) {

	d_gfx_init((d_gfx_desc) {
		.width = WIDTH,
		.height = HEIGHT,
		.clear_color = colori(0, 0, 0, 255),
	});

	img = d_img_load(d_res_path("res/wizard.png"));

}

void frame(void) {

	if (d_app_key_pressed(D_KEY_ESC)) {
		d_app_quit();
	}

	if (d_app_key_pressed(D_KEY_F)) {
		d_app_set_fullscreen(!d_app_fullscreen());
	}

	if (d_app_mouse_pressed(D_MOUSE_LEFT)) {
		pos = d_gfx_mouse_pos();
	}

	d_gfx_clear();

	vec2 mpos = d_gfx_mouse_pos();

	d_blit_bg();

	d_blit_img(&img, pos);
	d_blit_circle(mpos, 3, colorx(0xffffffff));
	d_blit_text("oh hi", pos, colorx(0xffffffff), false, false);

	d_gfx_present();

	d_app_set_title(fmt("%d", d_app_fps()));

}

int main(void) {
	d_app_run((d_app_desc) {
		.title = "sprite",
		.init = init,
		.frame = frame,
		.width = WIDTH * SCALE,
		.height = HEIGHT * SCALE,
	});
}
