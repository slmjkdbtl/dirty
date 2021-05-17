// wengwengweng

#define D_IMPL
#define D_CPU
#include <d_plat.h>
#include <d_math.h>
#include <d_app.h>
#include <d_gfx.h>

#define FMT_MAX 256

static const char *d_fmt(const char *fmt, ...) {

	static char buf[FMT_MAX];
	va_list args;

	va_start(args, fmt);
	vsnprintf(buf, FMT_MAX, fmt, args);
	va_end(args);

	return buf;

}

void init() {
	d_gfx_init((d_gfx_desc) {
		.width = 240,
		.height = 240,
		.clear_color = colori(0, 0, 0, 255),
	});
}

void frame() {

	if (d_app_key_pressed(D_KEY_ESC)) {
		d_app_quit();
	}

	d_gfx_clear();
	d_gfx_t_push();
	d_gfx_t_move(d_gfx_mouse_pos());
	d_gfx_t_rot_z(d_app_time());
// 	d_draw_line(vec2f(d_gfx_width() / 2, d_gfx_height() / 2), d_gfx_mouse_pos(), colori(0, 0, 255, 255));
// 	d_draw_tri(vec2f(40, 40), vec2f(60, 120), d_gfx_mouse_pos(), colori(0, 0, 255, 255));
	for (int i = 0; i < 10; i++) {
		d_draw_rect(vec2f(10, 10), vec2f(40, 40), colorx(0xffff00ff));
// 		d_draw_tri(vec2f(60, 20), vec2f(20, 60), vec2f(100, 80), colori(0, 0, 255, 255));
	}
	d_gfx_present();

	d_app_set_title(d_fmt("%d", d_app_fps()));
	d_gfx_t_pop();

}

int main() {
	d_app_run((d_app_desc) {
		.title = "hi",
		.init = init,
		.frame = frame,
		.width = 480,
		.height = 480,
	});
}
