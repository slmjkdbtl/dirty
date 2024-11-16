#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define D_IMPL
#include <d_plat.h>
#include <d_fs.h>
#include <d_math.h>
#include <d_app.h>
#include <d_gfx.h>
#include <d_tween.h>

#define WIDTH 640
#define HEIGHT 480
#define SCALE 1
#define NUM_FISH 640

typedef struct {
	bool is_pink;
	d_t2 t;
	float timer;
	float squeeze_time;
	bool squeezing;
	d_tween squeeze_tween;
} fish_t;

typedef struct {
	fish_t* fish;
	d_vec2 offset;
} grab_ctx_t;

bool debug = false;
d_img fish_img;
d_img pfish_img;
d_rect fish_rect;
fish_t pool[NUM_FISH];
grab_ctx_t grabbing;

fish_t fish_new(d_vec2 pos, bool is_pink) {
	return (fish_t) {
		.is_pink = is_pink,
		.t = (d_t2) {
			.pos = pos,
			.scale = V(0.2),
			.rot = R(0, 360),
			.origin = d_vec2_scale(V(fish_img.width, fish_img.height), 0.5),
		},
		.timer = 0,
		.squeeze_time = R(1, 5),
		.squeezing = false,
	};

}

void fish_update(fish_t* f) {
	float dt = d_app_dt();
	if (f->squeezing) {
		d_tween_update(&f->squeeze_tween, dt, &f->t.rot);
		if (f->squeeze_tween.done) {
			f->squeezing = false;
		}
	} else {
		f->timer += dt;
		if (f->timer >= f->squeeze_time) {
			f->timer = 0;
			f->squeeze_time = R(1, 5);
			float orig_rot = f->t.rot;
			f->t.rot += R() > 0.5 ? 3 : -3;
			f->squeeze_tween = d_tween_new(
				f->t.rot,
				orig_rot,
				1,
				NULL,
				d_ease_out_elastic
			);
			f->squeezing = true;
		}
	}
}

void fish_draw(fish_t* f) {
	d_transform_push();
	d_t2_apply(f->t);
	d_draw_img(f->is_pink ? &pfish_img : &fish_img);
	d_transform_pop();
}

void init(void) {

	d_gfx_init((d_gfx_desc) {
		.width = WIDTH,
		.height = HEIGHT,
		.clear_color = d_colori(0, 0, 0, 255),
	});

	fish_img = d_img_load(d_res_path("res/fish.png"));
	pfish_img = d_img_load(d_res_path("res/pfish.png"));
	fish_rect = (d_rect) { V(0), V(fish_img.width, fish_img.height) };

	for (int i = 0; i < NUM_FISH; i++) {
		float pad = 32;
		float x = R(pad, d_gfx_width() - pad);
		float y = R(pad, d_gfx_height() - pad);
		pool[i] = fish_new(V(x, y), i == 7);
	}

	grabbing.fish = NULL;
	grabbing.offset = V(0);

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

	if (d_app_key_pressed(D_KEY_TAB)) {
		debug = !debug;
	}

	if (d_app_mouse_pressed(D_MOUSE_LEFT)) {
		for (int i = NUM_FISH - 1; i >= 0; i--) {
			fish_t f = pool[i];
			d_poly body = d_poly_transform(
				d_rect_to_poly(fish_rect),
				d_t2_get_mat4(f.t)
			);
			if (d_col_pt_poly(mpos, body)) {
				memmove(
					pool + i,
					pool + i + 1,
					(NUM_FISH - i - 1) * sizeof(fish_t)
				);
				pool[NUM_FISH - 1] = f;
				grabbing.fish = &pool[NUM_FISH - 1];
				grabbing.offset = d_vec2_sub(mpos, f.t.pos);
				if (f.is_pink) {
					// TODO
				}
				break;
			}
		}
	}

	if (d_app_mouse_released(D_MOUSE_LEFT)) {
		grabbing.fish = NULL;
		grabbing.offset = V(0);
	}

	if (grabbing.fish) {
		grabbing.fish->t.pos = d_vec2_sub(mpos, grabbing.offset);
	}

	d_gfx_clear();

	for (int i = 0; i < NUM_FISH; i++) fish_update(&pool[i]);
	for (int i = 0; i < NUM_FISH; i++) fish_draw(&pool[i]);

	d_gfx_present();

}

int main(void) {
	d_app_run((d_app_desc) {
		.title = "Find The Pink Fish",
		.init = init,
		.frame = frame,
		.width = WIDTH * SCALE,
		.height = HEIGHT * SCALE,
	});
}
