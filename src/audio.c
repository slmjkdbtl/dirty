// wengwengweng

#include <SDL2/SDL.h>
#include <stb/stb_vorbis.c>

#include <dirty/dirty.h>

#define FREQ 44100
#define CHANNELS 1
#define SAMPLES 1024
#define MAX_TRACKS 256

typedef struct {
	SDL_AudioDeviceID dev;
	d_sound_pb tracks[MAX_TRACKS];
	int ntracks;
} d_audio_t;

static d_audio_t d_audio;

static void stream(void *udata, Uint8 *buf, int len) {

	float *fbuf = (float*)buf;

	for (int i = 0; i < SAMPLES; i++) {

		float frame = 0.0;

		for (int j = 0; j < d_audio.ntracks; j++) {

			d_sound_pb *p = &d_audio.tracks[j];

			if (p->paused || p->done) {
				continue;
			}

			if (p->src->samples == NULL) {
				p->done = true;
				continue;
			}

			if (p->pos + p->src->channels >= p->src->len) {
				if (p->loop) {
					p->pos = 0;
				} else {
					p->done = true;
					continue;
				}
			}

			float f = 0.0;

			for (int k = 0; k < p->src->channels; k++) {
				f += (float)p->src->samples[p->pos] / (float)SHRT_MAX;
				p->pos++;
			}

			frame += (f / (float)p->src->channels) * p->volume;

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

d_sound d_parse_sound(const unsigned char *bytes, int size) {

	int channels;
	int sample_rate;
	short *samples;
	int len = stb_vorbis_decode_memory(bytes, size, &channels, &sample_rate, &samples);

	if (len <= 0) {
		d_fail("failed to decode audio\n");
	}

	return (d_sound) {
		.channels = channels,
		.sample_rate = sample_rate,
		.samples = samples,
		.len = len,
	};

}

d_sound d_load_sound(const char *path) {

	int size;
	unsigned char *bytes = d_fread_b(path, &size);
	d_sound snd = d_parse_sound(bytes, size);
	free(bytes);

	return snd;

}

d_sound_pb* d_play(const d_sound *snd) {

	d_sound_pb src = (d_sound_pb) {
		.src = snd,
		.pos = 0,
		.loop = false,
		.paused = false,
		.volume = 1.0,
		.done = false,
	};

	for (int i = 0; i < d_audio.ntracks; i++) {
		if (d_audio.tracks[i].done) {
			d_audio.tracks[i] = src;
			return &d_audio.tracks[i];
		}
	}

	d_audio.tracks[d_audio.ntracks] = src;

	return &d_audio.tracks[d_audio.ntracks++];

}

void d_free_sound(d_sound *snd) {
	free(snd->samples);
	snd->samples = NULL;
}

// d_track d_parse_track(const unsigned char *bytes, int size) {

// 	stb_vorbis *decoder = stb_vorbis_open_memory(bytes, size, NULL, NULL);

// 	if (!decoder) {
// 		fprintf(stderr, "failed to decode audio\n");
// 		d_quit();
// 	}

// 	return (d_track) {
// 		.decoder = decoder,
// 	};

// }

// d_track d_load_track(const char *path) {

// 	int size;
// 	unsigned char *bytes = d_fread_b(path, &size);
// 	d_track track = d_parse_track(bytes, size);
// 	free(bytes);

// 	return track;

// }

// void d_track_play(const d_track *t) {
// }

// void d_free_track(d_track *t) {
// 	stb_vorbis_close(t->decoder);
// 	t->decoder = NULL;
// }

