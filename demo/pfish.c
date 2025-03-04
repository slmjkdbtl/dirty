#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_VORBIS_IMPLEMENTATION
#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>

#define D_IMPL
#include <d_plat.h>
#include <d_fs.h>
#include <d_math.h>
#include <d_app.h>
#include <d_gfx.h>
#include <d_tween.h>
#include <d_audio.h>

#define WIDTH 640
#define HEIGHT 480
#define SCALE 1
#define NUM_FISH 333

typedef struct {
	bool is_pink;
	d_t2 t;
	float timer;
	float squeeze_time;
	bool squeezing;
	d_tween squeeze_tween;
} Fish;

typedef struct {
	Fish* fish;
	d_vec2 offset;
} Grab;

bool debug = false;
d_img fish_img;
d_img pfish_img;
d_sound pop_sound;
d_rect fish_rect;
Fish pool[NUM_FISH];
Grab grabbing;
float squeeze_timer = 0;
float squeeze_time = 0;

Fish fish_new(d_vec2 pos, bool is_pink) {
	return (Fish) {
		.is_pink = is_pink,
		.t = (d_t2) {
			.pos = pos,
			.scale = V(0.3),
			.rot = R(0, 360),
			.origin = d_vec2_scale(V(fish_img.width, fish_img.height), 0.5),
		},
		.squeezing = false,
	};

}

void fish_squeeze(Fish* f) {
	d_play_ex(&pop_sound, (d_play_opts) {
		.speed = R(0.2, 1),
		.volume = R(0.5, 1.5),
	});
	float orig_rot = f->t.rot;
	f->t.rot += R() > 0.5 ? 0.2 : -0.2;
	f->squeeze_tween = d_tween_new(
		f->t.rot,
		orig_rot,
		1,
		NULL,
		d_ease_out_elastic
	);
	f->squeezing = true;
}

void fish_update(Fish* f) {
	float dt = d_app_dt();
	if (f->squeezing) {
		d_tween_update(&f->squeeze_tween, dt, &f->t.rot);
		if (f->squeeze_tween.done) {
			f->squeezing = false;
		}
	}
}

void fish_draw(Fish* f) {
	d_transform_push();
	d_t2_apply(f->t);
	d_draw_img(f->is_pink ? &pfish_img : &fish_img);
	d_transform_pop();
}

void init(void) {

	d_gfx_init((d_gfx_desc) {
		.scale = SCALE,
		.clear_color = d_colori(0, 0, 0, 255),
	});

	d_audio_init((d_audio_desc) {0});

	fish_img = d_img_load(d_res_path("res/fish.png"));
	pfish_img = d_img_load(d_res_path("res/pfish.png"));
	pop_sound = d_sound_load(d_res_path("res/pop.ogg"));
	fish_rect = (d_rect) { V(0), V(fish_img.width, fish_img.height) };

	for (int i = 0; i < NUM_FISH; i++) {
		float pad = 64;
		float x = R(pad, d_gfx_width() - pad);
		float y = R(pad, d_gfx_height() - pad);
		pool[i] = fish_new(V(x, y), i == 7);
	}

	grabbing.fish = NULL;
	grabbing.offset = V(0);

	squeeze_time = R(0, 1);

}

#define FMT_MAX 256

static char* fmt(char *fmt, ...) {

	static char buf[FMT_MAX];
	va_list args;

	va_start(args, fmt);
	vsnprintf(buf, FMT_MAX, fmt, args);
	va_end(args);

	return buf;

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
			Fish f = pool[i];
			d_poly body = d_poly_transform(
				d_rect_to_poly(fish_rect),
				d_t2_get_mat4(f.t)
			);
			if (d_col_pt_poly(mpos, body)) {
				memmove(
					pool + i,
					pool + i + 1,
					(NUM_FISH - i - 1) * sizeof(Fish)
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

	squeeze_timer += dt;

	if (squeeze_timer >= squeeze_time) {
		int idx = R(0, NUM_FISH);
		fish_squeeze(&pool[idx]);
		squeeze_time = R(0, 0.5);
		squeeze_timer = 0.0;
	}

	d_app_set_title(fmt("%d", d_app_fps()));

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
		.width = WIDTH,
		.height = HEIGHT,
	});
}
