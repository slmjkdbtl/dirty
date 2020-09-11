// wengwengweng

#ifndef D_AUDIO_H
#define D_AUDIO_H

// #define STB_VORBIS_HEADER_ONLY
// #include <stb/stb_vorbis.c>

// a static sound
typedef struct {
	int channels;
	int sample_rate;
	short *samples;
	int len;
} d_sound;

// sound playback control handle
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

// sound
d_sound d_parse_sound(const unsigned char *bytes, int size);
d_sound d_load_sound(const char *path);
d_sound_pb* d_play(const d_sound *sound);
void d_free_sound(d_sound *sound);

// d_track d_parse_track(const unsigned char *bytes, int size);
// d_track d_load_track(const char *path);
// void d_track_play(const d_track *track);
// void d_free_track(d_track *track);

#endif

