// wengwengweng

#ifndef D_AUDIO_H
#define D_AUDIO_H

#define D_SAMPLE_RATE 44100
#define D_MAX_EFFECTS 8

// a static sound
typedef struct {
	float *frames;
	int num_frames;
} d_sound;

// sound play config
typedef struct {
	bool loop;
	float volume;
	bool paused;
	float time;
	float (*effects[D_MAX_EFFECTS])(float f, void *udata);
	void *effects_udata[D_MAX_EFFECTS];
	int num_effects;
} d_play_conf;

// sound playback control handle
typedef struct {
	const d_sound *src;
	int pos;
	bool loop;
	float volume;
	bool paused;
	bool done;
	float (*effects[D_MAX_EFFECTS])(float f, void *udata);
	void *effects_udata[D_MAX_EFFECTS];
	int num_effects;
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

// user provided stream
void d_stream(float (*f)());

// SOUND
d_sound d_make_sound(const float *frames, int len);
d_sound d_parse_sound(const unsigned char *bytes, int size);
d_sound d_load_sound(const char *path);
float d_sound_sample(d_sound *snd, float time);
void d_free_sound(d_sound *sound);
// play a sound, returning a handle for control
d_playback *d_play(const d_sound *sound);
d_playback *d_play_ex(const d_sound *sound, d_play_conf conf);
void d_playback_seek(d_playback *pb, float time);
float d_playback_time(d_playback *pb);

// SYNTH
void d_synth_play(int note);
void d_synth_release(int note);
d_envelope *d_synth_envelope();
void d_synth_wav(float (*func)(float freq, float t));
float d_synth_peek(int n);

// voice
d_voice d_make_voice();
void d_voice_process(d_voice *v, const d_envelope *e, float dt);

// built in wave forms
float d_wav_sin(float freq, float t);
float d_wav_square(float freq, float t);
float d_wav_tri(float freq, float t);
float d_wav_saw(float freq, float t);
float d_wav_noise(float freq, float t);

#endif

