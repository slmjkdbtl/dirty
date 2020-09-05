// wengwengweng

#include <SDL2/SDL.h>
#include <stb/stb_vorbis.c>

#include <dirty/dirty.h>

#define FREQ 44100
#define CHANNELS 1
#define SAMPLES 1024
#define MAX_SOURCES 256

typedef struct {
	const short* samples;
	int channels;
	int sample_rate;
	int pos;
	int len;
} d_playback;

typedef struct {
	SDL_AudioDeviceID dev;
	d_playback pbs[MAX_SOURCES];
	int pb_count;
} d_audio_t;

static d_audio_t d_audio;

static void stream(void* udata, Uint8* buf, int len) {

	float* fbuf = (float*)buf;

	for (int i = 0; i < SAMPLES; i++) {

		float frame = 0.0;

		for (int j = 0; j < d_audio.pb_count; j++) {

			d_playback* pb = &d_audio.pbs[j];

			if (pb->pos + pb->channels >= pb->len) {
				// TODO: remove
				continue;
			}

			float f = 0.0;

			for (int k = 0; k < pb->channels; k++) {
				f += (float)pb->samples[pb->pos] / (float)SHRT_MAX;
				pb->pos++;
			}

			frame += f / (float)pb->channels;

		}

		fbuf[i] = frame;

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

d_sound d_parse_sound(const unsigned char* bytes, int size) {

	int channels;
	int sample_rate;
	short* samples;
	int len = stb_vorbis_decode_memory(bytes, size, &channels, &sample_rate, &samples);

	if (len <= 0) {
		fprintf(stderr, "failed to decode audio\n");
		d_quit();
	}

	return (d_sound) {
		.channels = channels,
		.sample_rate = sample_rate,
		.samples = samples,
		.len = len,
	};

}

d_sound d_load_sound(const char* path) {

	int size;
	unsigned char* content = d_fread_b(path, &size);
	d_sound snd = d_parse_sound(content, size);
	free(content);

	return snd;

}

void d_sound_play(const d_sound* snd) {
	d_playback pb = (d_playback) {
		.samples = snd->samples,
		.pos = 0,
		.len = snd->len,
		.channels = snd->channels,
		.sample_rate = snd->sample_rate,
	};
	d_audio.pbs[d_audio.pb_count] = pb;
	d_audio.pb_count++;
}

void d_free_sound(d_sound* snd) {
	free(snd->samples);
	snd->samples = NULL;
}

