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

typedef struct {
	SDL_AudioDeviceID device;
	d_sound_pb tracks[MAX_TRACKS];
	int num_tracks;
	float (*user_stream)();
	d_synth synth;
} d_audio_t;

static d_audio_t d_audio;

static float d_audio_next() {

	float frame = 0.0;

	for (int j = 0; j < d_audio.num_tracks; j++) {

		d_sound_pb *p = &d_audio.tracks[j];

		if (p->paused || p->done) {
			continue;
		}

		if (p->src->samples == NULL) {
			p->done = true;
			continue;
		}

		if (p->pos + p->src->channels >= p->src->num_samples) {
			if (p->loop) {
				p->pos = 0;
			} else {
				p->done = true;
				continue;
			}
		}

		// proceed through frames, make everything mono
		float f = 0.0;

		for (int k = 0; k < p->src->channels; k++) {
			f += p->src->samples[p->pos];
			p->pos++;
		}

		frame += (f / (float)p->src->channels) * p->volume;

	}

	frame += d_synth_next();

	if (d_audio.user_stream) {
		frame += d_audio.user_stream();
	}

	return clampf(frame, -1.0, 1.0);

}

static void sdl_stream(void *udata, unsigned char *buf, int len) {

	float *fbuf = (float*)buf;

	for (int i = 0; i < SAMPLES; i++) {
		fbuf[i] = d_audio_next();
	}

}

void d_audio_init() {

	SDL_AudioSpec spec = (SDL_AudioSpec) {
		.freq = SAMPLE_RATE,
		.format = AUDIO_F32,
		.channels = CHANNELS,
		.samples = SAMPLES,
		.callback = sdl_stream,
		.userdata = NULL,
	};

	d_audio.synth = d_make_synth();
	d_audio.device = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, SDL_AUDIO_ALLOW_ANY_CHANGE);
	SDL_PauseAudioDevice(d_audio.device, 0);

}

void d_audio_cleanup() {
	SDL_CloseAudioDevice(d_audio.device);
}

void d_stream(float (*f)()) {
	d_audio.user_stream = f;
}

// TODO: d_make_sound()

d_sound d_parse_sound(const unsigned char *bytes, int size) {

	int channels;
	int sample_rate;
	short *samples;
	int num_samples = stb_vorbis_decode_memory(bytes, size, &channels, &sample_rate, &samples);

	d_assert(num_samples > 0, "failed to decode audio\n");

	// TODO: convert to default sample rate and channel

	float *fsamples = malloc(sizeof(float) * num_samples);

	for (int i = 0; i < num_samples; i++) {
		fsamples[i] = (float)samples[i] / SHRT_MAX;
	}

	free(samples);

	return (d_sound) {
		.channels = channels,
		.samples = fsamples,
		.num_samples = num_samples,
	};

}

d_sound d_load_sound(const char *path) {

	size_t size;
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

	for (int i = 0; i < d_audio.num_tracks; i++) {
		if (d_audio.tracks[i].done) {
			d_audio.tracks[i] = src;
			return &d_audio.tracks[i];
		}
	}

	d_audio.tracks[d_audio.num_tracks] = src;

	return &d_audio.tracks[d_audio.num_tracks++];

}

void d_sound_pb_seek(d_sound_pb *pb, float time) {
	float len = pb->src->num_samples * SAMPLE_RATE;
	time = clampf(time, 0.0, len);
	pb->pos = (int)(time * SAMPLE_RATE);
}

void d_free_sound(d_sound *snd) {
	free(snd->samples);
	snd->samples = NULL;
}

float d_note_freq(int n) {
	return A4_FREQ * pow(powf(2.0, 1.0 / 12.0), n - A4_NOTE);
}

float d_wav_sin(float freq, float t) {
	return sin(freq * 2.0 * D_PI * t);
}

float d_wav_square(float freq, float t) {
	return d_wav_sin(freq, t) > 0.0 ? 1.0 : -1.0;
}

float d_wav_tri(float freq, float t) {
	return asin(d_wav_sin(freq, t)) * 2.0 / D_PI;
}

float d_wav_saw(float freq, float t) {
	return (2.0 / D_PI) * (freq * D_PI * fmod(t, 1.0 / freq) - D_PI / 2.0);
}

float d_wav_noise(float freq, float t) {
	return randf(-1.0, 1.0);
}

d_synth d_make_synth() {
	return (d_synth) {
		.notes = {0},
		.volume = 0.5,
		.clock = 0,
		.sample_rate = SAMPLE_RATE,
		.wav_func = d_wav_sin,
		.envelope = (d_envelope) {
			.attack = 0.05,
			.decay = 0.05,
			.sustain = 1.0,
			.release = 0.5,
		},
	};
}

d_voice d_make_voice() {
	return (d_voice) {
		.active = true,
		.life = 0.0,
		.afterlife = 0.0,
		.volume = 0.0,
		.alive = true,
	};
}

void d_synth_play(int note) {
	d_assert(note >= 0 && note < D_SYNTH_NOTES, "note out of bound: '%d'\n", note);
	d_audio.synth.notes[note] = d_make_voice();
}

void d_synth_release(int note) {
	d_assert(note >= 0 && note < D_SYNTH_NOTES, "note out of bound: '%d'\n", note);
	d_audio.synth.notes[note].active = false;
}

void d_voice_process(d_voice *v, const d_envelope *e, float dt) {

	if (!v->alive) {
		return;
	}

	float a = e->attack;
	float d = e->decay;
	float s = e->sustain;
	float r = e->release;

	// attack
	if (v->life <= a) {
		if (a == 0.0) {
			v->volume = 1.0;
		} else {
			v->volume = v->life / a;
		}
	// decay
	} else if (v->life > a && v->life <= a + d) {
		v->volume = 1.0 - (v->life - a) / d * (1.0 - s);
	// systain
	} else {
		if (v->active) {
			v->volume = s;
		// release
		} else {
			if (r == 0.0) {
				v->volume = 0.0;
			} else {
				v->volume = s * (1.0 - (v->afterlife / r));
				if (v->volume <= 0.0) {
					v->alive = false;
				}
			}
		}
	}

	v->life += dt;

	if (!v->active) {
		v->afterlife += dt;
	}

}

float d_synth_next() {

	d_synth *synth = &d_audio.synth;
	float frame = 0.0;
	float dt = 1.0 / (float)synth->sample_rate;

	synth->clock += dt;

	for (int i = 0; i < D_SYNTH_NOTES; i++) {

		d_voice *v = &synth->notes[i];

		d_voice_process(v, &synth->envelope, dt);

		float freq = d_note_freq(i);
		float sample = synth->wav_func(freq, synth->clock) * v->volume;

		frame += sample;

	}

	frame *= synth->volume;
	frame = clampf(frame, -1.0, 1.0);

	if (synth->buf_size < D_SYNTH_BUF_SIZE) {
		synth->buf[synth->buf_size++] = frame;
	} else {
		synth->buf[synth->buf_head++] = frame;
		if (synth->buf_head >= D_SYNTH_BUF_SIZE) {
			synth->buf_head = 0;
		}
	}

	return frame;

}

float d_synth_peek(int n) {
	d_synth *synth = &d_audio.synth;
	if (synth->buf_size == 0) {
		return 0.0;
	}
	int idx = (n + synth->buf_size - 1 + synth->buf_head) % D_SYNTH_BUF_SIZE;
	if (idx < 0 || idx >= D_SYNTH_BUF_SIZE) {
		return 0.0;
	}
	return synth->buf[idx];
}

d_envelope *d_synth_envelope() {
	return &d_audio.synth.envelope;
}

void d_synth_wav(float (*func)(float freq, float t)) {
	d_audio.synth.wav_func = func;
}

