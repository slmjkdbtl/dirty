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

#define WIDTH 120
#define HEIGHT 120
#define SCALE 4

d_model btfly;
vec2 pos;
d_tween_vec2 tween;
bool active;
bool show_bbox;

void init(void) {

	d_gfx_init((d_gfx_desc) {
		.width = WIDTH,
		.height = HEIGHT,
		.clear_color = colori(0, 0, 0, 255),
	});

	pos = vec2f(d_gfx_width() / 2.0, d_gfx_height() / 2.0);
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
		d_app_set_fullscreen(!d_app_is_fullscreen());
	}

	if (d_app_mouse_pressed(D_MOUSE_LEFT)) {
		active = true;
		vec2 mpos = d_gfx_mouse_pos();
		tween = d_tween_vec2_new(pos, mpos, 1, d_ease_out_elastic);
	}

	float dt = d_app_dt();

	if (active) {
		pos = d_tween_vec2_update(&tween, dt);
	}

	d_gfx_clear();

	d_blit_bg();

	d_gfx_t_push();
	d_gfx_t_move3(vec3f(pos.x, pos.y, 0));
	d_gfx_t_rot_y(0.5);
	d_gfx_t_rot_z(-0.8);
	d_gfx_t_scale3(vec3f(1, -1, 1));
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
		.title = "tween",
		.init = init,
		.frame = frame,
		.width = WIDTH * SCALE,
		.height = HEIGHT * SCALE,
	});
}
