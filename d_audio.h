// TODO: QOI audio
// TODO: 2 channels

#ifndef D_AUDIO_H
#define D_AUDIO_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct {
	float (*stream)(void);
} d_audio_desc;

// a static sound
typedef struct {
	short* frames;
	int num_frames;
} d_sound;

// sound play options
typedef struct {
	bool loop;
	float volume;
	float pitch;
	bool paused;
	float time;
} d_play_opts;

// sound playback control handle
typedef struct {
	d_sound* src;
	int pos;
	bool loop;
	float volume;
	float pitch;
	bool paused;
	bool done;
} d_playback;

typedef struct {
	float attack;
	float decay;
	float sustain;
	float release;
} d_envelope;

typedef struct {
	float life;
	float afterlife;
	float volume;
	bool active;
	bool alive;
} d_voice;

void d_audio_init(d_audio_desc);
void d_audio_dispose(void);

// SOUND
d_sound d_sound_new(short* frames, int num_frames);
d_sound d_sound_parse(uint8_t* bytes, size_t size);
#ifdef D_FS_H
d_sound d_sound_load(char* path);
#endif
float d_sound_sample(d_sound* snd, float time);
float d_sound_len(d_sound* snd);
void d_sound_free(d_sound* sound);
// play a sound, returning a handle for control
d_playback* d_play(d_sound* sound);
d_playback* d_play_ex(d_sound* sound, d_play_opts opts);
void d_playback_seek(d_playback* pb, float time);
float d_playback_time(d_playback* pb);

// SYNTH
void d_synth_play(int note);
void d_synth_release(int note);
d_envelope* d_synth_envelope(void);
void d_synth_wav(float (*func)(float freq, float t));
float d_synth_peek(int n);

// voice
d_voice d_voice_new(void);
void d_voice_process(d_voice* v, d_envelope* e, float dt);

// built in wave forms
float d_wav_sin(float freq, float t);
float d_wav_square(float freq, float t);
float d_wav_tri(float freq, float t);
float d_wav_saw(float freq, float t);
float d_wav_noise(float freq, float t);

#endif

#ifdef D_IMPL
#define D_AUDIO_IMPL
#endif

#ifdef D_AUDIO_IMPL
#ifndef D_AUDIO_IMPL_ONCE
#define D_AUDIO_IMPL_ONCE

#include <limits.h>
#include <math.h>

#if defined(__APPLE__)
	#define D_COREAUDIO
#elif defined(__EMSCRIPTEN__)
	#define D_WEBAUDIO
#elif defined(_WIN32) || defined(_WIN64)
	#define D_WASAPI
#elif defined(__ANDROID__)
	#define D_AAUDIO
#elif defined(__linux__) || defined(__unix__)
	#define D_ALSA
#endif

#if defined(D_COREAUDIO)
	#include <AudioToolbox/AudioToolbox.h>
#elif defined(D_ALSA)
#elif defined(D_WEBAUDIO)
	#include <emscripten/emscripten.h>
#endif

#define D_SAMPLE_RATE 44100
#define D_NUM_CHANNELS 1
#define D_BUFFER_FRAMES 2048
#define D_MAX_PLAYBACKS 1024
#define D_A4_FREQ 440
#define D_A4_NOTE 69
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

d_synth d_synth_new(void);
float d_synth_next(void);

typedef struct {
	d_playback playbacks[D_MAX_PLAYBACKS];
	int num_playbacks;
	float volume;
	float (*user_stream)(void);
	d_synth synth;
#if defined(D_COREAUDIO)
	AudioQueueRef queue;
#endif
} d_audio_ctx;

static d_audio_ctx d_audio;

static float d_audio_next(void) {

	float frame = 0.0;

	for (int i = 0; i < d_audio.num_playbacks; i++) {

		d_playback* p = &d_audio.playbacks[i];

		if (p->done) {
			d_audio.playbacks[i] = d_audio.playbacks[d_audio.num_playbacks - 1];
			d_audio.num_playbacks--;
			i--;
			continue;
		}

		if (p->paused) {
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

		frame += (float)p->src->frames[p->pos] / SHRT_MAX * p->volume;
		p->pos++;

	}

	frame += d_synth_next();

	if (d_audio.user_stream) {
		frame += d_audio.user_stream();
	}

	return frame * d_audio.volume;

}

#if defined(D_COREAUDIO)

static void d_ca_stream(void* udata, AudioQueueRef queue, AudioQueueBufferRef buffer) {

	int num_frames = buffer->mAudioDataByteSize / (sizeof(float) * D_NUM_CHANNELS);
	float* data = (float*)buffer->mAudioData;

	for (int i = 0; i < num_frames; i++) {
		data[i] = d_audio_next();
	}

	AudioQueueEnqueueBuffer(queue, buffer, 0, NULL);

}

static void d_ca_init(void) {

	AudioStreamBasicDescription fmt = {
		.mSampleRate = D_SAMPLE_RATE,
		.mFormatID = kAudioFormatLinearPCM,
		.mFormatFlags = 0
			| kLinearPCMFormatFlagIsFloat
			| kAudioFormatFlagIsPacked
			,
		.mFramesPerPacket = 1,
		.mChannelsPerFrame = D_NUM_CHANNELS,
		.mBytesPerFrame = sizeof(float) * D_NUM_CHANNELS,
		.mBytesPerPacket = sizeof(float) * D_NUM_CHANNELS,
		.mBitsPerChannel = 32,
	};

	AudioQueueRef queue;

	AudioQueueNewOutput(
		&fmt,
		d_ca_stream,
		NULL,
		NULL,
		NULL,
		0,
		&queue
	);

	for (int i = 0; i < 2; i++) {

		int buf_size = D_BUFFER_FRAMES * fmt.mBytesPerFrame;
		AudioQueueBufferRef buf;

		AudioQueueAllocateBuffer(queue, buf_size, &buf);
		buf->mAudioDataByteSize = buf_size;
		memset(buf->mAudioData, 0, buf->mAudioDataByteSize);

		AudioQueueEnqueueBuffer(queue, buf, 0, NULL);

	}

	AudioQueueStart(queue, NULL);

	d_audio.queue = queue;

}

static void d_ca_dispose(void) {
	AudioQueueStop(d_audio.queue, true);
	AudioQueueDispose(d_audio.queue, true);
}

#endif // D_COREAUDIO

#if defined(D_WEBAUDIO)

EMSCRIPTEN_KEEPALIVE float d_cjs_audio_next(void) {
	return d_audio_next();
}

EM_JS(void, d_js_webaudio_init, (int sample_rate, int buf_size), {

	const ctx = new (window.AudioContext || window.webkitAudioContext)({
		sampleRate: sample_rate,
	});

	dirty.audioCtx = ctx;

	const scriptNode = ctx.createScriptProcessor(buf_size, 1, 1);

	// TODO: has artifact
	scriptNode.addEventListener("audioprocess", (e) => {
		const output = e.outputBuffer.getChannelData(0);
		for (let i = 0; i < buf_size; i++) {
			output[i] = _d_cjs_audio_next();
		}
	});

	scriptNode.connect(ctx.destination);
	ctx.resume();
	document.addEventListener("click", () => ctx.resume());
	document.addEventListener("keydown", () => ctx.resume());

})

static void d_webaudio_init(void) {
	d_js_webaudio_init(D_SAMPLE_RATE, D_BUFFER_FRAMES);
}

#endif

#if defined(D_ALSA)

static void d_alsa_init(void) {
	// snd_pcm_t* dev = NULL;
	// snd_pcm_open(dev, "default", SND_PCM_STREAM_PLAYBACK, 0);
	// TODO
}

#endif // D_ALSA

void d_audio_init(d_audio_desc desc) {
	d_audio.volume = 1.0;
	d_audio.user_stream = desc.stream;
	d_audio.synth = d_synth_new();
#if defined(D_COREAUDIO)
	d_ca_init();
#elif defined(D_WEBAUDIO)
	d_webaudio_init();
#elif defined(D_ALSA)
	d_alsa_init();
#endif
}

void d_audio_dispose(void) {
#if defined(D_COREAUDIO)
	d_ca_dispose();
#endif
}

d_sound d_sound_new(short* frames, int num_frames) {
	int size = sizeof(short) * num_frames;
	short* frames_n = malloc(size);
	memcpy(frames_n, frames, size);
	return (d_sound) {
		.frames = frames_n,
		.num_frames = num_frames,
	};
}

d_sound d_sound_empty(void) {
	return (d_sound) {
		.num_frames = 0,
		.frames = malloc(0),
	};
}

static short *stereo_to_mono(
	const short *input,
	int input_len,
	int *output_len
) {
	*output_len = input_len / 2;
	short *output = malloc(*output_len * sizeof(short));
	for (int i = 0; i < input_len; i += 2) {
		output[i / 2] = (input[i] + input[i + 1]) / 2;
	}
	return output;
}

static short *resample(
	const short *input,
	int input_len,
	int input_rate,
	int output_rate,
	int *output_len
) {
	double ratio = (double) input_rate / output_rate;
	*output_len = input_len / ratio;
	short *output = malloc(*output_len * sizeof(short));
	for (int i = 0; i < *output_len; i++) {
		// find the corresponding position in the input buffer
		double src_pos = i * ratio;
		int index = (int)src_pos;
		double frac = src_pos - index;
		// linear interpolation with boundary check
		int interp_val;
		if (index + 1 < input_len) {
			// interpolate between two neighboring samples
			interp_val = (int)((1.0 - frac) * input[index] + frac * input[index + 1]);
		} else {
			// use the last sample if we're at the end of the input
			interp_val = input[index];
		}
		if (interp_val > SHRT_MAX) interp_val = SHRT_MAX;
		if (interp_val < SHRT_MIN) interp_val = SHRT_MIN;
		output[i] = (short)interp_val;
	}
	return output;
}

static uint8_t ogg_sig[] = { 0x4f, 0x67, 0x67, 0x53 };
static uint8_t wav_sig[] = { 0x52, 0x49, 0x46, 0x46 };

d_sound d_sound_parse(uint8_t* bytes, size_t size) {
	if (memcmp(bytes, ogg_sig, sizeof(ogg_sig)) == 0) {
#ifdef STB_VORBIS_IMPLEMENTATION
		d_sound snd = d_sound_empty();
		int num_channels, sample_rate;
		short *frames;
		int num_frames = stb_vorbis_decode_memory(
			bytes,
			size,
			&num_channels,
			&sample_rate,
			&frames
		);
		if (num_frames < 0) {
			fprintf(stderr, "failed to decode audio\n");
			return d_sound_empty();
		}
		if (num_channels == 2 && D_NUM_CHANNELS == 1) {
			short *new_frames = stereo_to_mono(
				frames,
				num_frames,
				&num_frames
			);
			free(frames);
			frames = new_frames;
		}
		if (sample_rate != D_SAMPLE_RATE) {
			short *new_frames = resample(
				frames,
				num_frames,
				sample_rate,
				D_SAMPLE_RATE,
				&num_frames
			);
			free(frames);
			frames = new_frames;
		}
		return (d_sound) {
			.num_frames = num_frames,
			.frames = frames,
		};
#else
		fprintf(stderr, "ogg support requires 'stb_vorbis.c' and STB_VORBIS_IMPLEMENTATION flag\n");
		return d_sound_empty();
#endif // #ifdef STB_VORBIS_IMPLEMENTATION
	} else {
		fprintf(stderr, "unsupported audio format\n");
		return d_sound_empty();
	}
}

#ifdef D_FS_H
d_sound d_sound_load(char* path) {
	size_t size;
	uint8_t* bytes = d_read_bytes(path, &size);
	if (!bytes) {
		fprintf(stderr, "failed to load sound from '%s'\n", path);
		return d_sound_empty();
	}
	d_sound snd = d_sound_parse(bytes, size);
	free(bytes);
	return snd;
}
#endif // #ifdef D_FS_H

float d_sound_sample(d_sound* snd, float time) {
	int pos = clampi(time * D_SAMPLE_RATE, 0, snd->num_frames - 1);
	return (float)snd->frames[pos] / SHRT_MAX;
}

float d_sound_len(d_sound* snd) {
	return (float)snd->num_frames / (float)D_SAMPLE_RATE;
}

void d_sound_free(d_sound* snd) {
	free(snd->frames);
	memset(snd, 0, sizeof(d_sound));
}

d_playback* d_play(d_sound* snd) {
	return d_play_ex(snd, (d_play_opts) {
		.loop = false,
		.paused = false,
		.volume = 1.0,
		.pitch = 1.0,
	});
}

d_playback* d_play_ex(d_sound* snd, d_play_opts opts) {
	int pos = clampi((int)(opts.time * D_SAMPLE_RATE), 0, snd->num_frames - 1);
	d_playback src = (d_playback) {
		.src = snd,
		.pos = pos,
		.loop = opts.loop,
		.paused = opts.paused,
		.volume = opts.volume,
		.pitch = opts.pitch,
		.done = false,
	};
	d_audio.playbacks[d_audio.num_playbacks] = src;
	return &d_audio.playbacks[d_audio.num_playbacks++];
}

void d_playback_seek(d_playback* pb, float time) {
	pb->pos = clampi(time * D_SAMPLE_RATE, 0, pb->src->num_frames - 1);
}

float d_playback_time(d_playback* pb) {
	return (float)pb->pos / (float)D_SAMPLE_RATE;
}

float d_note_freq(int n) {
	return D_A4_FREQ * pow(powf(2.0, 1.0 / 12.0), n - D_A4_NOTE);
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

#endif // #ifndef D_AUDIO_IMPL_ONCE
#endif // #ifdef D_AUDIO_IMPL
