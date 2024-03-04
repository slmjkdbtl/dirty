#define D_IMPL
#define D_CPU
#include <d_plat.h>
#include <d_math.h>
#include <d_app.h>
#include <d_gfx.h>

void init(void) {
	d_gfx_init((d_gfx_desc) {
		.width = 480,
		.height = 480,
		.clear_color = colori(0, 0, 0, 255),
	});
}

void frame(void) {

	if (d_app_key_pressed(D_KEY_ESC)) {
		d_app_quit();
	}

	d_gfx_clear();
	d_draw_line2(vec2f(0, 0), d_gfx_mouse_pos(), 3, colori(0, 0, 255, 255));
	d_draw_line2(vec2f(100, 100), d_gfx_mouse_pos(), 3, colori(0, 0, 255, 255));
	d_gfx_present();

}

int main(void) {
	d_app_run((d_app_desc) {
		.title = "line",
		.init = init,
		.frame = frame,
		.width = 480,
		.height = 480,
	});
}
