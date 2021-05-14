// wengwengweng

#define D_IMPL
#define D_CPU
#include <d_plat.h>
#include <d_math.h>
#include <d_app.h>
#include <d_gfx.h>

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
// 	d_draw_line(vec2f(d_gfx_width() / 2, d_gfx_height() / 2), d_gfx_mouse_pos(), colori(0, 0, 255, 255));
	d_draw_tri(vec2f(40, 40), vec2f(60, 120), d_gfx_mouse_pos(), colori(0, 0, 255, 255));
	d_gfx_present();

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
