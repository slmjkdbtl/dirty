// wengwengweng

#include <math.h>
#include <SDL2/SDL.h>
#include <stb/stb_vorbis.c>

#include <dirty/dirty.h>
#include "audio.h"

#define SAMPLE_RATE 44100
#define CHANNELS 1
#define SAMPLES 1024
#define MAX_TRACKS 256
#define A4_FREQ 440
#define A4_NOTE 69
#define PI 3.14159

typedef struct {
	SDL_AudioDeviceID dev;
	d_sound_pb tracks[MAX_TRACKS];
	int ntracks;
	d_synth synth;
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

		frame += d_synth_next();

		fbuf[i] = frame;

	}

}

void d_audio_init() {

	SDL_AudioSpec spec = (SDL_AudioSpec) {
		.freq = SAMPLE_RATE,
		.format = AUDIO_F32,
		.channels = CHANNELS,
		.samples = SAMPLES,
		.callback = stream,
		.userdata = NULL,
	};

	d_audio.synth = d_make_synth(SAMPLE_RATE);
	d_audio.dev = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, SDL_AUDIO_ALLOW_ANY_CHANGE);
	SDL_PauseAudioDevice(d_audio.dev, 0);

}

void d_audio_cleanup() {
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
	unsigned char *bytes = d_read_bytes(path, &size);
	d_sound snd = d_parse_sound(bytes, size);
	free(bytes);

	return snd;

}

d_sound_pb *d_play(const d_sound *snd) {

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

float d_note_freq(int n) {
	return A4_FREQ * pow(powf(2.0, 1.0 / 12.0), n - A4_NOTE);
}

d_synth d_make_synth(int rate) {
	return (d_synth) {
		.notes = {0},
		.volume = 0.0,
		.clock = 0,
		.sample_rate = rate,
		.envelope = (d_envelope) {
			.attack = 0.01,
			.decay = 0.01,
			.sustain = 1.0,
			.release = 1.0,
		},
	};
}

void d_synth_play(int note) {
	if (note < 0 || note >= D_SYNTH_NOTES) {
		d_fail("note out of bound: '%d'\n", note);
	}
	d_audio.synth.notes[note].active = true;
	d_audio.synth.notes[note].life = 1.0;
}

void d_synth_release(int note) {
	if (note < 0 || note >= D_SYNTH_NOTES) {
		d_fail("note out of bound: '%d'\n", note);
	}
	d_audio.synth.notes[note].active = false;
}

float d_synth_next() {

	d_synth *synth = &d_audio.synth;
	float t = (float)(synth->clock % synth->sample_rate) / (float)synth->sample_rate;
	float dt = 1.0 / (float)synth->sample_rate;
	float frame = 0.0;

	synth->clock += 1;

	for (int i = 0; i < D_SYNTH_NOTES; i++) {

		d_voice *v = &synth->notes[i];

		if (v->life <= 0.0) {
			continue;
		}

		v->life -= dt;

		int freq = (int)d_note_freq(i);

		frame += sin(freq * 2.0 * PI * t) * v->life;

	}

	return frame;

}

d_envelope *d_synth_envelope() {
	return &d_audio.synth.envelope;
}

