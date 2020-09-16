// wengwengweng

#ifndef D_AUDIO_INTERNAL_H
#define D_AUDIO_INTERNAL_H

typedef struct {
	float life;
	bool active;
} d_voice;

typedef struct {
	d_voice notes[D_SYNTH_NOTES];
	float volume;
	int sample_rate;
	unsigned long clock;
	d_envelope envelope;
} d_synth;

void d_audio_init();
void d_audio_cleanup();

d_synth d_make_synth();
float d_synth_next();

#endif

