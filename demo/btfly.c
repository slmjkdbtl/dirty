#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_VORBIS_IMPLEMENTATION
#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>
#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

#define D_IMPL
#include <d_plat.h>
#include <d_math.h>
#include <d_fs.h>
#include <d_app.h>
#include <d_gfx.h>
#include <d_audio.h>

#define WIDTH 480
#define HEIGHT 480
#define SCALE 4

d_model btfly;
d_sound snd;
bool show_bbox;

void init(void) {

	d_gfx_init((d_gfx_desc) {
		.scale = SCALE,
		.depth_test = true,
		.backface_cull = true,
		.clear_color = d_colori(0, 0, 0, 255),
	});

	d_audio_init((d_audio_desc) {0});

	btfly = d_model_load(d_res_path("res/btfly.glb"));
	snd = d_sound_load(d_res_path("res/pop.ogg"));

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

	if (d_app_key_pressed(D_KEY_SPACE)) {
		d_play(&snd);
	}

	d_gfx_clear();
	d_blit_bg();

	d_transform_push();
	d_transform_pos3(d_vec3f(d_gfx_width() / 2.0, d_gfx_height() / 2.0, 0));
	d_transform_rot_y(d_app_time() * 50);
	d_transform_rot_z(d_app_time() * -20);
	d_transform_scale3(d_vec3f(4, -4, 4));
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
		.title = "btfly",
		.init = init,
		.frame = frame,
		.width = WIDTH,
		.height = HEIGHT,
	});
}
