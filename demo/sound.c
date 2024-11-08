#define STB_VORBIS_IMPLEMENTATION
#include <stb_vorbis.c>

#define D_IMPL
#include <d_plat.h>
#include <d_fs.h>
#include <d_math.h>
#include <d_audio.h>

int main(void) {
	d_audio_init((d_audio_desc) {0});
	d_sound snd = d_sound_load(d_res_path("res/bark.ogg"));
	d_play(&snd);
	usleep((float)snd.num_frames / (float)D_SAMPLE_RATE * 1000000);
}
