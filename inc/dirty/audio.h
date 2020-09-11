// wengwengweng

#ifndef D_AUDIO_H
#define D_AUDIO_H

// #define STB_VORBIS_HEADER_ONLY
// #include <stb/stb_vorbis.c>

typedef struct {
	int channels;
	int sample_rate;
	short *samples;
	int len;
} d_sound;

typedef struct {
	const d_sound *src;
	int pos;
	bool loop;
	float volume;
	bool paused;
	bool done;
} d_sound_pb;

// typedef struct {
// 	stb_vorbis* decoder;
// } d_track;

d_sound d_parse_sound(const unsigned char*, int);
d_sound d_load_sound(const char*);
d_sound_pb* d_play(const d_sound*);
void d_free_sound(d_sound*);

// d_track d_parse_track(const unsigned char*, int);
// d_track d_load_track(const char*);
// void d_track_play(const d_track*);
// void d_free_track(d_track*);

#endif

