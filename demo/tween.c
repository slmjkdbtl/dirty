#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

#define D_IMPL
#include <d_plat.h>
#include <d_math.h>
#include <d_fs.h>
#include <d_app.h>
#include <d_gfx.h>
#include <d_tween.h>

#define WIDTH 480
#define HEIGHT 480
#define SCALE 2

d_tweener tweener;
d_model btfly;
d_vec2 pos;
d_vec2 rot;
bool show_bbox;

void init(void) {

	d_gfx_init((d_gfx_desc) {
		.scale = SCALE,
		.clear_color = d_colorx(0x00000000),
	});

	pos = d_vec2f(d_gfx_width() / 2.0, d_gfx_height() / 2.0);
	btfly = d_model_load(d_res_path("res/btfly.glb"));
	tweener = d_tweener_new(128);

}

void frame(void) {

	if (d_app_key_pressed(D_KEY_ESC)) {
		d_app_quit();
	}

	if (d_app_key_pressed(D_KEY_TAB)) {
		show_bbox = !show_bbox;
	}

	if (d_app_key_pressed(D_KEY_F)) {
		d_app_set_fullscreen(!d_app_is_fullscreen());
	}

	if (d_app_mouse_pressed(D_MOUSE_LEFT)) {
		d_vec2 mpos = d_gfx_mouse_pos();
		d_tweener_add_vec2(&tweener, pos, mpos, 1, &pos, d_ease_out_elastic);
	}

	if (d_app_mouse_down(D_MOUSE_LEFT)) {
		d_vec2 mdpos = d_gfx_mouse_dpos();
		rot.x -= mdpos.y / 100;
		rot.y -= mdpos.x / 100;
	}

	float dt = d_app_dt();
	d_vec2 mpos = d_gfx_mouse_pos();

	d_tweener_update(&tweener, dt);

	d_gfx_clear();
	// d_blit_bg();

	d_transform_push();
	d_transform_pos3((d_vec3) { pos.x, pos.y, 0 });
	d_transform_rot_x(-mpos.y / d_gfx_height() + 0.5);
	d_transform_rot_y(-mpos.x / d_gfx_width() + 0.5);
	d_transform_scale3((d_vec3) { 2, -2, 2 });
	d_transform_pos3(d_vec3_scale(btfly.center, -1));
	d_draw_model(&btfly);

	if (show_bbox) {
		d_draw_bbox(btfly.bbox, d_colorx(0x0000ffff));
	}

	d_transform_pop();

	d_gfx_present();

}

int main(void) {
	d_app_run((d_app_desc) {
		.title = "tween",
		.init = init,
		.frame = frame,
		.width = WIDTH,
		.height = HEIGHT,
		.borderless = true,
		.always_on_top = true,
	});
}
