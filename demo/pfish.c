// https://imgur.com/gallery/sudden-death-game-jam-official-spritesheet-FszGWNa

#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_VORBIS_IMPLEMENTATION
#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>

#define D_IMPL
#define D_SHORTCUT
#include <d_plat.h>
#include <d_fs.h>
#include <d_math.h>
#include <d_app.h>
#include <d_gfx.h>
#include <d_tween.h>
#include <d_audio.h>
#include <d_timer.h>

#define WIDTH 640
#define HEIGHT 480
#define SCALE 1
#define NUM_FISH 20
#define MAX_SQUEEZE_INTERVAL 40
#define SQUEEZE_INTENSITY 4

typedef struct {
	bool is_pink;
	d_t2 t;
	d_timer squeeze_timer;
	d_tween squeeze_tween;
	bool is_squeezing;
	bool is_grabbing;
} fish_t;

typedef struct {
	fish_t* fish;
	d_vec2 offset;
} grab_t;

bool debug = false;
d_img fish_img;
d_img pfish_img;
d_sound pop_sound;
d_rect fish_rect;
fish_t pool[NUM_FISH];
fish_t* pfish;
grab_t grabbing;
d_cam cam;

fish_t fish_new(d_vec2 pos, bool is_pink) {
	return (fish_t) {
		.is_pink = is_pink,
		.t = (d_t2) {
			.pos = pos,
			.scale = v2(1, 1),
			.rot = r(0, 360),
			.origin = d_vec2_scale(v2(fish_img.width, fish_img.height), 0.5),
		},
		.squeeze_timer = d_timer_new(r(0, MAX_SQUEEZE_INTERVAL), false),
		.is_squeezing = false,
		.is_grabbing = false,
	};

}

void fish_squeeze(fish_t* f) {
	d_play_ex(&pop_sound, (d_play_opts) {
		.speed = r(0.1, 1.2),
		.volume = r(0.5, 1.0),
	});
	float orig_rot = f->t.rot;
	f->t.rot += r(0, 1) > 0.5 ? -SQUEEZE_INTENSITY : SQUEEZE_INTENSITY;
	f->squeeze_tween = d_tween_new(
		f->t.rot,
		orig_rot,
		1,
		NULL,
		d_ease_out_elastic
	);
	f->is_squeezing = true;
}

void fish_update(fish_t* f) {
	float dt = d_app_dt();
	if (f->is_squeezing) {
		d_tween_update(&f->squeeze_tween, dt, &f->t.rot);
		if (f->squeeze_tween.done) {
			f->is_squeezing = false;
		}
	} else {
		if (d_timer_tick(&f->squeeze_timer, dt)) {
			fish_squeeze(f);
			d_timer_reset_to(&f->squeeze_timer, r(0, MAX_SQUEEZE_INTERVAL));
		}
	}
	if (f->is_grabbing) {
		f->t.rot += r(-3, 3);
	}
}

void fish_draw(fish_t* f) {
	d_transform_push();
	d_t2_apply(f->t);
	if (f->is_grabbing) {
		d_transform_pos(v2(r(-6, 6), r(-6, 6)));
	}
	d_draw_img(f->is_pink ? &pfish_img : &fish_img);
	if (debug) {
		d_draw_rect_outline(fish_rect.p1, fish_rect.p2, D_WHITE);
	}
	d_transform_pop();
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

void init(void) {

	d_gfx_init((d_gfx_desc) {
		.scale = SCALE,
		.clear_color = rgb(0, 0, 0),
	});

	d_audio_init((d_audio_desc) {0});

	fish_img = d_img_load(d_res_path("res/fish.png"));
	pfish_img = d_img_load(d_res_path("res/pfish.png"));
	pop_sound = d_sound_load(d_res_path("res/pop.ogg"));
	fish_rect = (d_rect) {
		{ 16, 4 },
		{ fish_img.width - 32, fish_img.height - 4 },
	};

	for (int i = 0; i < NUM_FISH; i++) {
		bool is_pink = i == 3;
		float pad = is_pink ? 200 : 100;
		float x = r(pad, d_gfx_width() - pad);
		float y = r(pad, d_gfx_height() - pad);
		pool[i] = fish_new(v2(x, y), is_pink);
	}

	grabbing.fish = NULL;
	grabbing.offset = v2(0, 0);

	cam.center = v2(d_gfx_width() / 2.0, d_gfx_height() / 2.0);
	cam.scale = 1.0;
	cam.rot = 0.0;

}

void frame(void) {

	float dt = d_app_dt();

	if (pfish) {
		// cam.scale = 3.0;
		// cam.center = pfish->t.pos;
	}

	d_mat4 cam_mat = d_cam_mat4(&cam);
	d_mat4 cam_mat_inv = d_mat4_invert(cam_mat);
	d_vec2 mpos = d_mat4_mult_vec2(cam_mat_inv, d_gfx_mouse_pos());
	printf("%f, %f\n", mpos.x, mpos.y);

	if (d_app_key_pressed(D_KEY_ESC)) {
		d_app_quit();
	}

	if (d_app_key_pressed(D_KEY_F)) {
		d_app_set_fullscreen(!d_app_is_fullscreen());
	}

	if (d_app_key_pressed(D_KEY_TAB)) {
		debug = !debug;
	}

	if (d_app_key_down(D_KEY_W)) {
		cam.center.y -= 400 * dt;
	}

	if (d_app_key_down(D_KEY_S)) {
		cam.center.y += 400 * dt;
	}

	if (d_app_key_down(D_KEY_A)) {
		cam.center.x -= 400 * dt;
	}

	if (d_app_key_down(D_KEY_D)) {
		cam.center.x += 400 * dt;
	}

	if (d_app_key_down(D_KEY_DOWN)) {
		cam.scale -= 2 * dt;
	}

	if (d_app_key_down(D_KEY_UP)) {
		cam.scale += 2 * dt;
	}

	if (d_app_key_down(D_KEY_LEFT)) {
		cam.rot -= 60 * dt;
	}

	if (d_app_key_down(D_KEY_RIGHT)) {
		cam.rot += 60 * dt;
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
				grabbing.fish->is_grabbing = true;
				if (f.is_pink) {
					pfish = &f;
				}
				break;
			}
		}
	}

	if (d_app_mouse_released(D_MOUSE_LEFT)) {
		if (grabbing.fish) {
			grabbing.fish->is_grabbing = false;
			grabbing.fish = NULL;
			grabbing.offset = v2(0, 0);
		}
	}

	if (grabbing.fish) {
		grabbing.fish->t.pos = d_vec2_sub(mpos, grabbing.offset);
	}

	d_app_set_title(fmt("%d", d_app_fps()));

	d_gfx_clear();
	d_transform_push();
	d_transform_apply(cam_mat);

	for (int i = 0; i < NUM_FISH; i++) fish_update(&pool[i]);
	for (int i = 0; i < NUM_FISH; i++) fish_draw(&pool[i]);

	d_transform_pop();
	d_gfx_present();

}

int main(void) {
	srand(time(NULL));
	d_app_run((d_app_desc) {
		.title = "Find The Pink Fish",
		.init = init,
		.frame = frame,
		.width = WIDTH,
		.height = HEIGHT,
	});
}
