// wengwengweng

#ifndef D_AUDIO_H
#define D_AUDIO_H

#include <SDL2/SDL.h>

typedef struct {
	SDL_AudioDeviceID dev;
} d_audio_t;

extern d_audio_t d_audio;

#endif

