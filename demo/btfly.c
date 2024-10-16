#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

#define D_IMPL
#define D_CPU
#include <d_plat.h>
#include <d_math.h>
#include <d_fs.h>
#include <d_app.h>
#include <d_gfx.h>

#define WIDTH 64
#define HEIGHT 64
#define SCALE 4

d_model btfly;
bool show_bbox;

void init(void) {

	d_gfx_init((d_gfx_desc) {
		.width = WIDTH,
		.height = HEIGHT,
		.clear_color = colori(0, 0, 0, 255),
	});

	btfly = d_model_load(d_res_path("res/btfly.glb"));

}

void frame(void) {

	if (d_app_key_pressed(D_KEY_ESC)) {
		d_app_quit();
	}

	if (d_app_key_pressed(D_KEY_TAB)) {
		show_bbox = !show_bbox;
	}

	if (d_app_key_pressed(D_KEY_F)) {
		d_app_set_fullscreen(!d_app_fullscreen());
	}

	d_gfx_clear();

	d_blit_bg();

	d_gfx_t_push();
	d_gfx_t_move3(vec3f(WIDTH / 2, HEIGHT / 2, 0));
	d_gfx_t_rot_y(d_app_time());
	d_gfx_t_rot_z(d_app_time() / 2);
	d_gfx_t_scale3(vec3f(3, -3, 3));
	d_gfx_t_move3(vec3_scale(btfly.center, -1));
	d_draw_model(&btfly);
	if (show_bbox) {
		d_draw_bbox(btfly.bbox, colorx(0x0000ffff));
	}
	d_gfx_t_pop();

	d_gfx_present();

}

int main(void) {
	d_app_run((d_app_desc) {
		.title = "btfly",
		.init = init,
		.frame = frame,
		.width = WIDTH * SCALE,
		.height = HEIGHT * SCALE,
	});
}
