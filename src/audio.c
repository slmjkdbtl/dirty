// wengwengweng

#include <math.h>
#include <SDL2/SDL.h>
#include <stb/stb_vorbis.c>

#include <dirty/dirty.h>

#define CHANNELS 1
#define SAMPLES 1024
#define MAX_PLAYBACKS 256
#define A4_FREQ 440
#define A4_NOTE 69
#define D_SYNTH_NOTES 128
#define D_SYNTH_BUF_SIZE 44100

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

d_synth d_make_synth();
float d_synth_next();

typedef struct {
	SDL_AudioDeviceID device;
	d_playback playbacks[MAX_PLAYBACKS];
	int num_playbacks;
	float (*user_stream)();
	d_synth synth;
} d_audio_ctx;

static d_audio_ctx d_audio;

static float d_audio_next() {

	float frame = 0.0;

	for (int j = 0; j < d_audio.num_playbacks; j++) {

		d_playback *p = &d_audio.playbacks[j];

		if (p->paused || p->done) {
			continue;
		}

		if (p->src->frames == NULL) {
			p->done = true;
			p->paused = true;
			continue;
		}

		if (p->pos >= p->src->num_frames) {
			if (p->loop) {
				p->pos = 0;
			} else {
				p->done = true;
				p->paused = true;
				continue;
			}
		}

		float f = p->src->frames[p->pos] * p->volume;

		for (int i = 0; i < p->num_effects; i++) {
			f = p->effects[i](f, p->effects_udata[i]);
		}

		frame += f;
		p->pos++;

	}

	frame += d_synth_next();

	if (d_audio.user_stream) {
		frame += d_audio.user_stream();
	}

	return frame;

}

static void sdl_stream(void *udata, unsigned char *buf, int len) {

	float *fbuf = (float*)buf;

	for (int i = 0; i < SAMPLES; i++) {
		fbuf[i] = clampf(d_audio_next(), -1.0, 1.0);
	}

}

void d_audio_init(d_desc *desc) {

	SDL_AudioSpec spec = (SDL_AudioSpec) {
		.freq = D_SAMPLE_RATE,
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

void d_audio_quit() {
	SDL_CloseAudioDevice(d_audio.device);
}

void d_stream(float (*f)()) {
	d_audio.user_stream = f;
}

d_sound d_make_sound(const float *frames, int len) {
	int size = sizeof(float) * len;
	float *fframes = malloc(size);
	memcpy(fframes, frames, size);
	return (d_sound) {
		.frames = fframes,
		.num_frames = len,
	};
}

d_sound d_parse_sound(const unsigned char *bytes, int size) {

	int channels;
	int sample_rate;
	short *frames;
	int num_frames = stb_vorbis_decode_memory(bytes, size, &channels, &sample_rate, &frames);

	d_assert(num_frames > 0, "failed to decode audio\n");

	int num_fframes = num_frames / channels;
	float *fframes = malloc(sizeof(float) * num_fframes);

	for (int i = 0; i < num_fframes; i++) {
		float frame = 0.0;
		for (int j = 0; j < channels; j++) {
			frame += (float)frames[i * channels + j] / SHRT_MAX;
		}
		fframes[i] = frame / channels;
	}

	free(frames);

	return (d_sound) {
		.frames = fframes,
		.num_frames = num_fframes,
	};

}

d_sound d_load_sound(const char *path) {

	size_t size;
	unsigned char *bytes = d_read_bytes(path, &size);
	d_sound snd = d_parse_sound(bytes, size);
	free(bytes);

	return snd;

}

float d_sound_sample(d_sound *snd, float time) {
	return snd->frames[clampi(time * D_SAMPLE_RATE, 0, snd->num_frames - 1)];
}

float d_sound_len(d_sound *snd) {
	return (float)snd->num_frames / (float)D_SAMPLE_RATE;
}

void d_free_sound(d_sound *snd) {
	free(snd->frames);
	snd->frames = NULL;
}

d_playback *d_play(const d_sound *snd) {
	return d_play_ex(snd, (d_play_conf) {
		.loop = false,
		.paused = false,
		.volume = 1.0,
		.effects = {0},
		.effects_udata = {0},
		.num_effects = 0,
	});
}

d_playback *d_play_ex(const d_sound *snd, d_play_conf conf) {

	int pos = clampi((int)(conf.time * D_SAMPLE_RATE), 0, snd->num_frames - 1);

	d_playback src = (d_playback) {
		.src = snd,
		.pos = pos,
		.loop = conf.loop,
		.paused = conf.paused,
		.volume = conf.volume,
		.done = false,
	};

	for (int i = 0; i < d_audio.num_playbacks; i++) {
		if (d_audio.playbacks[i].done) {
			d_audio.playbacks[i] = src;
			return &d_audio.playbacks[i];
		}
	}

	d_audio.playbacks[d_audio.num_playbacks] = src;

	return &d_audio.playbacks[d_audio.num_playbacks++];

}

void d_playback_seek(d_playback *pb, float time) {
	pb->pos = clampi(time * D_SAMPLE_RATE, 0, pb->src->num_frames - 1);
}

float d_playback_time(d_playback *pb) {
	return (float)pb->pos / (float)D_SAMPLE_RATE;
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

