#define D_IMPL
#include <d_plat.h>
#include <d_math.h>
#include <d_app.h>
#include <d_gfx.h>

void init(void) {
	d_gfx_init((d_gfx_desc) {
		.clear_color = d_colori(0, 0, 0, 255),
	});
}

void frame(void) {

	if (d_app_key_pressed(D_KEY_ESC)) {
		d_app_quit();
	}

	d_blit_text("123abc", (d_vec2) { 10, 10 }, d_colorx(0xffffffff), false, false);
	d_gfx_present();

}

int main(void) {
	d_app_run((d_app_desc) {
		.title = "hi",
		.init = init,
		.frame = frame,
		.width = 240,
		.height = 240,
	});
}
