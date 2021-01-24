// wengwengweng

#define D_IMPL
#define D_CPU
#include <d_plat.h>
#include <d_fs.h>
#include <d_math.h>
#include <d_app.h>
#include <d_gfx.h>

#define WIDTH 240
#define HEIGHT 240
#define SCALE 2

#define FMT_MAX 256

static const char *d_fmt(const char *fmt, ...) {

	static char buf[FMT_MAX];
	va_list args;

	va_start(args, fmt);
	vsnprintf(buf, FMT_MAX, fmt, args);
	va_end(args);

	return buf;

}

d_img img;
vec2 pos;

void init() {

	d_gfx_init((d_gfx_desc) {
		.width = WIDTH,
		.height = HEIGHT,
		.clear_color = colori(0, 0, 0, 255),
	});

	d_fs_init((d_fs_desc) {0});

	img = d_load_img("res/wizard.dspr");

}

void frame() {

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

// 	for (int i = 0; i < 2000; i++) {
		d_draw_img(&img, pos);
// 	}
// 	d_draw_rect(vec2f(0, 0), d_mouse_pos(), colori(0, 0, 255, 100));
	d_draw_circle(d_gfx_mouse_pos(), 3, colori(0, 255, 255 ,255));
	d_draw_text("oh hi", pos);

	d_gfx_present();

	d_app_set_title(d_fmt("%d", d_app_fps()));

}

int main() {
	d_app_run((d_app_desc) {
		.title = "app",
		.init = init,
		.frame = frame,
		.width = WIDTH * SCALE,
		.height = HEIGHT * SCALE,
	});
}

