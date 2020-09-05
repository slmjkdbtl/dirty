// wengwengweng

#ifndef D_AUDIO_H
#define D_AUDIO_H

typedef struct {
	int channels;
	int sample_rate;
	short* samples;
	int len;
} d_sound;

d_sound d_parse_sound(const unsigned char*, int);
d_sound d_load_sound(const char*);
void d_sound_play(const d_sound*);
void d_free_sound(d_sound*);

#endif

