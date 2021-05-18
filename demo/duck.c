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

#define WIDTH 120
#define HEIGHT 120

#define FMT_MAX 256

static const char *d_fmt(const char *fmt, ...) {

	static char buf[FMT_MAX];
	va_list args;

	va_start(args, fmt);
	vsnprintf(buf, FMT_MAX, fmt, args);
	va_end(args);

	return buf;

}

d_model duck;

void init() {

	d_gfx_init((d_gfx_desc) {
		.width = WIDTH,
		.height = HEIGHT,
		.clear_color = colori(0, 0, 0, 255),
	});

	d_fs_init((d_fs_desc) {0});

	duck = d_load_model("res/duck.glb");

}

void frame() {

	if (d_app_key_pressed(D_KEY_ESC)) {
		d_app_quit();
	}

	d_gfx_clear();

	d_gfx_t_push();
	d_gfx_t_move3(vec3f(WIDTH / 2, 100, 0));
	d_gfx_t_rot_y(d_app_time());
	d_gfx_t_scale3(vec3f(0.5, -0.5, 0.5));
	d_draw_model(&duck);
	d_gfx_t_pop();

	d_gfx_present();

	d_app_set_title(d_fmt("%d", d_app_fps()));

}

int main() {
	d_app_run((d_app_desc) {
		.title = "app",
		.init = init,
		.frame = frame,
		.width = WIDTH,
		.height = HEIGHT,
	});
}
