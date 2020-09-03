// wengwengweng

#include <math.h>

#include <dirty/dirty.h>

#define FREQ 44100
#define CHANNELS 1
#define SAMPLES 1024

d_audio_t d_audio;

static void stream(void* udata, Uint8* buf, int len) {

	float* fbuf = (float*)buf;
	static float t = 0.0;

	for (int i = 0; i < SAMPLES; i++) {
// 		float frame = sin(t);
// 		fbuf[i] = frame;
		fbuf[i] = 0.0;
		t += 0.05;
	}

}

void d_audio_init() {

	SDL_AudioSpec spec = (SDL_AudioSpec) {
		.freq = FREQ,
		.format = AUDIO_F32,
		.channels = CHANNELS,
		.samples = SAMPLES,
		.callback = stream,
		.userdata = NULL,
	};

	d_audio.dev = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, SDL_AUDIO_ALLOW_ANY_CHANGE);
	SDL_PauseAudioDevice(d_audio.dev, 0);

}

void d_audio_destroy() {
	SDL_CloseAudioDevice(d_audio.dev);
}

