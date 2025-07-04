#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

#define D_IMPL
#include <d_plat.h>
#include <d_fs.h>
#include <d_math.h>
#include <d_app.h>
#include <d_gfx.h>

#define WIDTH 240
#define HEIGHT 240
#define SCALE 2

#define FMT_MAX 256

d_model duck;
d_model btfly;

void init(void) {

	d_gfx_init((d_gfx_desc) {
		.width = WIDTH,
		.height = HEIGHT,
		.clear_color = d_colori(0, 0, 0, 255),
	});

	btfly = d_model_load(d_res_path("res/btfly.glb"));

}

void frame(void) {

	if (d_app_key_pressed(D_KEY_ESC)) {
		d_app_quit();
	}

	if (d_app_key_pressed(D_KEY_F)) {
		d_app_set_fullscreen(!d_app_is_fullscreen());
	}

	d_gfx_clear();

	d_gfx_set_bbuf_write(true);

	d_transform_push();
	d_transform_pos3(d_vec3f(60, 80, 0));
	d_transform_rot_y(d_app_time() * 60);
	d_transform_scale3(d_vec3f(6, -6, 6));
	d_draw_model(&btfly);
	d_transform_pop();

	d_vec2 mpos = d_gfx_mouse_pos();
	float t = d_app_time();

	d_gfx_set_bbuf_test(true);
	if (d_gfx_bbuf_get(mpos.x, mpos.y)) {
		d_blit_rect(
			d_vec2f(0, 0),
			d_vec2f(d_gfx_width(), d_gfx_height()),
			d_colori((sin(t * 10) + 1) / 2 * 255, (cos(t * 10) + 1) / 2 * 255, 255, 255)
		);
	} else {
		d_blit_bg();
	}
	d_gfx_set_bbuf_test(false);
	d_gfx_bbuf_clear();

	d_gfx_present();

}

int main(void) {
	d_app_run((d_app_desc) {
		.title = "3d",
		.init = init,
		.frame = frame,
		.width = WIDTH * SCALE,
		.height = HEIGHT * SCALE,
	});
}
