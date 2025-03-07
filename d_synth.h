#ifndef D_SYNTH_H
#define D_SYNTH_H

#define D_SYNTH_NOTES 128
#define D_A4_FREQ 440
#define D_A4_NOTE 69
#define D_SYNTH_BUF_SIZE 44100

#include <stdbool.h>

typedef struct {
	float life;
	float afterlife;
	float volume;
	bool active;
	bool alive;
} d_voice;

typedef struct {
	float attack;
	float decay;
	float sustain;
	float release;
} d_envelope;

typedef struct {
	d_voice notes[D_SYNTH_NOTES];
	float volume;
	int sample_rate;
	float clock;
	d_envelope envelope;
	float buf[D_SYNTH_BUF_SIZE];
	int buf_head;
	int buf_size;
	float (*wav_func)(float freq, float t);
} d_synth;

d_synth d_synth_new(void);
float d_synth_next(void);
void d_synth_play(int note);
void d_synth_release(int note);
d_envelope* d_synth_envelope(void);
void d_synth_wav(float (*func)(float freq, float t));
float d_synth_peek(int n);

d_voice d_voice_new(void);
void d_voice_process(d_voice* v, d_envelope* e, float dt);

float d_wav_sin(float freq, float t);
float d_wav_square(float freq, float t);
float d_wav_tri(float freq, float t);
float d_wav_saw(float freq, float t);
float d_wav_noise(float freq, float t);

#endif

#ifdef D_IMPL
#define D_SYNTH_IMPL
#endif

#ifdef D_SYNTH_IMPL
#ifndef D_SYNTH_IMPL_ONCE
#define D_SYNTH_IMPL_ONCE

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

float d_note_freq(int n) {
	return D_A4_FREQ * pow(powf(2.0, 1.0 / 12.0), n - D_A4_NOTE);
}

float d_wav_sin(float freq, float t) {
	return sin(freq * 2.0 * M_PI * t);
}

float d_wav_square(float freq, float t) {
	return d_wav_sin(freq, t) > 0.0 ? 1.0 : -1.0;
}

float d_wav_tri(float freq, float t) {
	return asin(d_wav_sin(freq, t)) * 2.0 / M_PI;
}

float d_wav_saw(float freq, float t) {
	return (2.0 / M_PI) * (freq * M_PI * fmod(t, 1.0 / freq) - M_PI / 2.0);
}

float d_wav_noise(float freq, float t) {
	return d_randf(-1.0, 1.0);
}

d_synth d_synth_new(void) {
	return (d_synth) {
		.notes = {0},
		.volume = 0.5,
		.clock = 0,
		.sample_rate = D_SAMPLE_RATE,
		.wav_func = d_wav_sin,
		.envelope = (d_envelope) {
			.attack = 0.05,
			.decay = 0.05,
			.sustain = 1.0,
			.release = 0.5,
		},
	};
}

d_voice d_voice_new(void) {
	return (d_voice) {
		.active = true,
		.life = 0.0,
		.afterlife = 0.0,
		.volume = 0.0,
		.alive = true,
	};
}

void d_synth_play(int note) {
	if (note < 0 || note >= D_SYNTH_NOTES) {
		fprintf(stderr, "note out of bound: '%d'\n", note);
		return;
	}
	d_audio.synth.notes[note] = d_voice_new();
}

void d_synth_release(int note) {
	if (note < 0 || note >= D_SYNTH_NOTES) {
		fprintf(stderr, "note out of bound: '%d'\n", note);
		return;
	}
	d_audio.synth.notes[note].active = false;
}

void d_voice_process(d_voice* v, d_envelope* e, float dt) {

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
	} else if (v->life > a && v->life <= a + d) {
		// decay
		v->volume = 1.0 - (v->life - a) / d * (1.0 - s);
	} else {
		// systain
		if (v->active) {
			v->volume = s;
		} else {
			// release
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

float d_synth_next(void) {

	d_synth* synth = &d_audio.synth;
	float frame = 0.0;
	float dt = 1.0 / (float)synth->sample_rate;

	synth->clock += dt;

	for (int i = 0; i < D_SYNTH_NOTES; i++) {

		d_voice* v = &synth->notes[i];

		d_voice_process(v, &synth->envelope, dt);

		float freq = d_note_freq(i);
		float sample = synth->wav_func(freq, synth->clock) * v->volume;

		frame += sample;

	}

	frame *= synth->volume;

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
	d_synth* synth = &d_audio.synth;
	if (synth->buf_size == 0) {
		return 0.0;
	}
	int idx = (n + synth->buf_size - 1 + synth->buf_head) % D_SYNTH_BUF_SIZE;
	if (idx < 0 || idx >= D_SYNTH_BUF_SIZE) {
		return 0.0;
	}
	return synth->buf[idx];
}

d_envelope* d_synth_envelope(void) {
	return &d_audio.synth.envelope;
}

void d_synth_wav(float (*func)(float freq, float t)) {
	d_audio.synth.wav_func = func;
}

#endif
#endif
