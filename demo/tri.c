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

#define WIDTH 480
#define HEIGHT 480

#define FMT_MAX 256

static char* fmt(char *fmt, ...) {

	static char buf[FMT_MAX];
	va_list args;

	va_start(args, fmt);
	vsnprintf(buf, FMT_MAX, fmt, args);
	va_end(args);

	return buf;

}

void init() {

	d_gfx_init((d_gfx_desc) {
		.width = WIDTH,
		.height = HEIGHT,
		.clear_color = colori(0, 0, 0, 255),
	});

}

void frame() {

	if (d_app_key_pressed(D_KEY_ESC)) {
		d_app_quit();
	}

	if (d_app_key_pressed(D_KEY_F)) {
		d_app_set_fullscreen(!d_app_fullscreen());
	}

	d_gfx_clear();

	for (int i = 0; i < 20; i++) {

		d_draw_prim_tri(
			(d_vertex) {
				.pos = vec3f(240, 120, 0),
				.color = colorx(0xffffffff),
			},
			(d_vertex) {
				.pos = vec3f(120, 320, 0),
				.color = colorx(0xffffffff),
			},
			(d_vertex) {
				.pos = vec3f(360, 320, 0),
				.color = colorx(0xffffffff),
			},
			NULL
		);

	}

	d_gfx_present();

	d_app_set_title(fmt("%d", d_app_fps()));

}

int main() {
	d_app_run((d_app_desc) {
		.title = "tri",
		.init = init,
		.frame = frame,
		.width = WIDTH,
		.height = HEIGHT,
	});
}
