// wengwengweng

#define D_IMPL
#define D_CPU
#include <d_plat.h>
#include <d_fs.h>
#include <d_math.h>
#include <d_audio.h>
#include <d_app.h>
#include <d_gfx.h>

d_img img;
d_sound snd;
d_sound track;
d_playback *track_pb;

void init() {

	d_gfx_init((d_gfx_desc) {
		.width = 240,
		.height = 240,
		.clear_color = colori(0, 0, 0, 255),
	});

	d_fs_init((d_fs_desc) {
		.path = "demo/res",
	});

	d_audio_init((d_audio_desc) {0});

	img = d_load_img("wizard.dspr");
	snd = d_load_sound("shoot.dsnd");
	track = d_load_sound("yo.dsnd");
	track_pb = d_play(&track);

}

void frame() {

	if (d_app_key_pressed(D_KEY_ESC)) {
		d_app_quit();
	}

	if (d_app_key_pressed(D_KEY_SPACE)) {
		track_pb->paused = !track_pb->paused;
	}

	if (d_app_key_pressed(D_KEY_Q)) {
		d_play(&snd);
	}

	d_gfx_clear();
	d_draw_img(&img, d_app_mouse_pos());
	d_draw_text("hi", d_app_mouse_pos());
	d_gfx_present();

}

int main() {
	d_app_run((d_app_desc) {
		.title = "app",
		.init = init,
		.frame = frame,
		.width = 240,
		.height = 240,
	});
}

