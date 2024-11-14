// TODO: no img

#define D_IMPL
#include <d_plat.h>
#include <d_math.h>
#include <d_app.h>
#include <d_gfx.h>

#define WIDTH 480
#define HEIGHT 480

void init(void) {

	d_gfx_init((d_gfx_desc) {
		.width = WIDTH,
		.height = HEIGHT,
		.clear_color = d_colori(0, 0, 0, 255),
	});

}

void frame(void) {

	if (d_app_key_pressed(D_KEY_ESC)) {
		d_app_quit();
	}

	d_gfx_clear();

	d_draw_prim_quad(
		(d_vertex) {
			.pos = d_vec3f(0, 0, 0),
			.color = d_colorx(0xff0000ff),
		},
		(d_vertex) {
			.pos = d_vec3f(0, d_gfx_height(), 0),
			.color = d_colorx(0xffff00ff),
		},
		(d_vertex) {
			.pos = d_vec3f(d_gfx_width(), d_gfx_height(), 0),
			.color = d_colorx(0x0000ffff),
		},
		(d_vertex) {
			.pos = d_vec3f(d_gfx_width(), 0, 0),
			.color = d_colorx(0x00ff00ff),
		},
		NULL
	);

	d_gfx_present();

}

int main(void) {
	d_app_run((d_app_desc) {
		.title = "quad",
		.init = init,
		.frame = frame,
		.width = WIDTH,
		.height = HEIGHT,
	});
}
