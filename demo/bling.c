#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define D_IMPL
#include <d_plat.h>
#include <d_fs.h>
#include <d_math.h>
#include <d_app.h>
#include <d_gfx.h>

#define WIDTH 240
#define HEIGHT 240
#define SCALE 2
#define MAX_BLINGS 1024

d_img img;
float timer = 0.0;
float bling_time = 0.5f;

typedef struct {
	d_t2 t;
	bool dead;
	float scale;
} bling_t;

bling_t blings[MAX_BLINGS];
int num_blings;

void init(void) {

	d_gfx_init((d_gfx_desc) {
		.width = WIDTH,
		.height = HEIGHT,
		.clear_color = d_colori(0, 0, 0, 255),
	});

	img = d_img_load(d_res_path("res/bling.png"));

}

void frame(void) {

	float dt = d_app_dt();
	d_vec2 mpos = d_gfx_mouse_pos();

	if (d_app_key_pressed(D_KEY_ESC)) {
		d_app_quit();
	}

	if (d_app_key_pressed(D_KEY_F)) {
		d_app_set_fullscreen(!d_app_is_fullscreen());
	}

	d_gfx_clear();

	if (d_app_mouse_moved()) {
		timer += dt;
		if (timer >= bling_time) {
			timer = 0.0f;
			float dx = d_randf(-4, 4);
			float dy = d_randf(-4, 4);
			blings[num_blings++] = (bling_t) {
				.t = (d_t2) {
					.pos = d_vec2_add(mpos, d_vec2f(dx, dy)),
					.rot = 0,
					.scale = d_vec2f(0, 0),
					.origin = d_vec2_scale(d_vec2f(img.width, img.height), 0.5),
				},
				.dead = false,
			};
		}
	}

	for (int i = 0; i < num_blings; i++) {
		bling_t b = blings[i];
		if (b.dead) {
			// TODO
			continue;
		}
	}

	d_gfx_present();

}

int main(void) {
	d_app_run((d_app_desc) {
		.title = "bling",
		.init = init,
		.frame = frame,
		.width = WIDTH * SCALE,
		.height = HEIGHT * SCALE,
	});
}
