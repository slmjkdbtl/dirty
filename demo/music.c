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

#define WIDTH 480
#define HEIGHT 480
#define SCALE 2

#define FMT_MAX 256

static char* fmt(char *fmt, ...) {

	static char buf[FMT_MAX];
	va_list args;

	va_start(args, fmt);
	vsnprintf(buf, FMT_MAX, fmt, args);
	va_end(args);

	return buf;

}

d_sound music;
d_playback* music_pb;

void init(void) {

	d_gfx_init((d_gfx_desc) {
		.scale = SCALE,
		.clear_color = rgb(0, 0, 0),
	});

	d_audio_init((d_audio_desc) {0});

	music = d_sound_load(d_res_path("res/cowpoke.ogg"));
	music_pb = d_play_ex(&music, (d_play_opts) {
		.speed = 1.0,
		.volume = 1.0,
		.loop = true,
	});

}

void frame(void) {

	float dt = d_app_dt();
	d_vec2 mpos = d_gfx_mouse_pos();

	if (d_app_key_pressed(D_KEY_ESC)) {
		d_app_quit();
	}

	if (d_app_key_pressed(D_KEY_SPACE)) {
		music_pb->paused = !music_pb->paused;
	}

	if (d_app_key_pressed(D_KEY_LEFT)) {
		// music_pb->speed -= 0.1;
		d_playback_seek_by(music_pb, -1);
	}

	if (d_app_key_pressed(D_KEY_RIGHT)) {
		// music_pb->speed += 0.1;
		d_playback_seek_by(music_pb, 1);
	}

	if (d_app_key_pressed(D_KEY_DOWN)) {
		music_pb->volume -= 0.1;
	}

	if (d_app_key_pressed(D_KEY_UP)) {
		music_pb->volume += 0.1;
	}

	d_gfx_clear();

	d_blit_text(fmt("volume: %.02f", music_pb->volume), v2(10, 10), D_WHITE, false, false);
	d_blit_text(fmt("speed: %.02f", music_pb->speed), v2(10, 20), D_WHITE, false, false);
	d_blit_text(fmt("progress: %.02f / %.02f", d_playback_time(music_pb), d_sound_duration(&music)), v2(10, 30), D_WHITE, false, false);

	d_gfx_present();

}

int main(void) {
	d_app_run((d_app_desc) {
		.title = "music",
		.init = init,
		.frame = frame,
		.width = WIDTH,
		.height = HEIGHT,
	});
}

