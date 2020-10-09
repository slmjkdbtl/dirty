// wengwengweng

#ifndef D_AUDIO_INTERNAL_H
#define D_AUDIO_INTERNAL_H

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

void d_audio_init();
void d_audio_quit();

d_synth d_make_synth();
float d_synth_next();

#endif

