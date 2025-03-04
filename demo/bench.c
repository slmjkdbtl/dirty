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

#define FMT_MAX 1024

static char* fmt(char *fmt, ...) {

	static char buf[FMT_MAX];
	va_list args;

	va_start(args, fmt);
	vsnprintf(buf, FMT_MAX, fmt, args);
	va_end(args);

	return buf;

}

d_img img;

void init(void) {

	d_gfx_init((d_gfx_desc) {
		.width = WIDTH,
		.height = HEIGHT,
		.clear_color = d_colori(0, 0, 0, 255),
	});

	img = d_img_load(d_res_path("res/wizard.png"));

}

void frame(void) {

	if (d_app_key_pressed(D_KEY_ESC)) {
		d_app_quit();
	}

	if (d_app_key_pressed(D_KEY_F)) {
		d_app_set_fullscreen(!d_app_is_fullscreen());
	}

	d_gfx_clear();
	d_blit_bg();

	int count = 5000;

	for (int i = 0; i < count; i++) {
		d_transform_push();
		d_transform_pos(d_vec2f(d_randf(0, d_gfx_width()), d_randf(0, d_gfx_height())));
		d_transform_pos(d_vec2f(-img.width / 2.0, -img.height / 2.0));
		d_draw_img(&img);
		d_transform_pop();
	}

	char* fps = fmt("%d", d_app_fps());
	d_blit_text(fps, d_vec2f(10, 10), d_colorx(0xffffffff), false, false);
	d_app_set_title(fps);

	d_gfx_present();

}

int main(void) {
	d_app_run((d_app_desc) {
		.title = "bench",
		.init = init,
		.frame = frame,
		.width = WIDTH * SCALE,
		.height = HEIGHT * SCALE,
	});
}
