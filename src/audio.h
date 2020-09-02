// wengwengweng

typedef struct {
	SDL_AudioDeviceID dev;
} d_audio_t;

extern d_audio_t d_audio;

void d_audio_init();
void d_audio_destroy();

