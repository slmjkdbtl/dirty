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
d_model duck;
d_model btfly;
vec2 pos;

void init() {

	d_gfx_init((d_gfx_desc) {
		.width = WIDTH,
		.height = HEIGHT,
		.clear_color = colori(0, 0, 0, 255),
	});

	d_fs_init((d_fs_desc) {0});

	img = d_load_img("res/wizard.png");
	duck = d_load_model("res/duck.glb");
	btfly = d_load_model("res/btfly.glb");

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

	vec2 mpos = d_gfx_mouse_pos();

	d_draw_prim_quad(
		(d_vertex) {
			.pos = vec3f(0, 0, 0),
			.color = colorx(0xff0000ff),
		},
		(d_vertex) {
			.pos = vec3f(d_gfx_width(), 0, 0),
			.color = colorx(0x00ff00ff),
		},
		(d_vertex) {
			.pos = vec3f(d_gfx_width(), d_gfx_height(), 0),
			.color = colorx(0x0000ffff),
		},
		(d_vertex) {
			.pos = vec3f(0, d_gfx_height(), 0),
			.color = colorx(0xffff00ff),
		},
		NULL
	);

// 	for (int i = 0; i < 6000; i++) {
		d_draw_img(&img, pos);
// 	}
// 	d_draw_circle(mpos, 3, colorx(0xffffffff));
// 	d_draw_text("oh hi", pos, colorx(0xffffffff));

// 	for (int i = 0; i < 100; i++) {
		d_gfx_t_push();
		d_gfx_t_move3(vec3f(120, 200, mpos.y));
		d_gfx_t_rot_y(d_app_time());
		d_gfx_t_scale3(vec3f(600, -600, 600));
		d_draw_model(&btfly);
		d_gfx_t_pop();
// 	}

		d_gfx_t_push();
		d_gfx_t_move3(vec3f(120, 200, 0));
		d_gfx_t_rot_y(d_app_time());
		d_gfx_t_scale3(vec3f(1, -1, 1));
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
		.width = WIDTH * SCALE,
		.height = HEIGHT * SCALE,
	});
}

