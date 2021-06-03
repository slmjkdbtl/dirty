// wengwengweng

#define D_IMPL
#include <d_plat.h>
#include <d_fs.h>
#include <d_math.h>
#include <d_audio.h>

int main() {
	d_fs_init((d_fs_desc) {0});
	d_audio_init((d_audio_desc) {0});
	d_sound snd = d_load_sound("res/yo.dsnd");
	d_play(&snd);
	sleep(snd.num_frames / D_SAMPLE_RATE);
}
