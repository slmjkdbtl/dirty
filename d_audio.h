// TODO: QOA audio
// TODO: 2 channels?

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
	float speed;
	bool paused;
	float time;
} d_play_opts;

// sound playback control handle
typedef struct {
	d_sound* src;
	float pos;
	float speed;
	bool loop;
	float volume;
	bool paused;
	bool done;
} d_playback;

void d_audio_init(d_audio_desc);
void d_audio_dispose(void);

// SOUND
d_sound d_sound_new(short* frames, int num_frames);
d_sound d_sound_parse(uint8_t* bytes, size_t size);
#ifdef D_FS_H
d_sound d_sound_load(char* path);
#endif
d_sound d_sound_clone(d_sound* snd);
d_sound d_sound_stereo_to_mono(d_sound* snd);
d_sound d_sound_resample(d_sound* src, int input_rate, int output_rate);
float d_sound_sample(d_sound* snd, float time);
float d_sound_duration(d_sound* snd);
void d_sound_free(d_sound* sound);
// play a sound, returning a handle for control
d_playback* d_play(d_sound* sound);
d_playback* d_play_ex(d_sound* sound, d_play_opts opts);
void d_playback_seek(d_playback* pb, float time);
void d_playback_seek_by(d_playback* pb, float time);
float d_playback_time(d_playback* pb);

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
#include <alsa/asoundlib.h>
#elif defined(D_WASAPI)
#include <windows.h>
#include <audioclient.h>
#include <mmdeviceapi.h>
#elif defined(D_WEBAUDIO)
#include <emscripten/emscripten.h>
#endif

#define D_SAMPLE_RATE 44100
#define D_NUM_CHANNELS 1
#define D_BUFFER_FRAMES 2048
#define D_MAX_PLAYBACKS 1024

typedef struct {
	d_playback playbacks[D_MAX_PLAYBACKS];
	int num_playbacks;
	float volume;
	float (*user_stream)(void);
#if defined(D_COREAUDIO)
	AudioQueueRef queue;
#elif defined(D_WASAPI)
	IMMDeviceEnumerator* device_enumerator;
	IMMDevice* device;
	IAudioClient* client;
	IAudioRenderClient* render_client;
	UINT32 num_buf_frames;
	HANDLE thread;
	HANDLE event;
#endif
} d_audio_ctx;

static d_audio_ctx d_audio;

static float d_audio_next(void) {

	float frame = 0.0;

	for (int i = 0; i < d_audio.num_playbacks; i++) {

		d_playback* p = &d_audio.playbacks[i];

		if (p->done) {
			memmove(
				d_audio.playbacks + i,
				d_audio.playbacks + i + 1,
				(d_audio.num_playbacks - i - 1) * sizeof(d_playback)
			);
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

		int idx = (int)p->pos;
		float frac = p->pos - idx;
		float f = p->src->frames[idx];
		float val = f;
		if (idx + 1 < p->src->num_frames) {
			val = (1.0 - frac) * f + frac * p->src->frames[idx + 1];
		}
		frame += val / SHRT_MAX * p->volume;
		p->pos += p->speed;

	}

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

// -------------------------------------------------------------
// Windows
#if defined(D_WASAPI)

static DWORD WINAPI d_wasapi_thread(LPVOID lpParam) {
	HRESULT hr;
	while (1) {
		WaitForSingleObject(d_audio.event, INFINITE);
		UINT32 num_frames = 0;
		hr = d_audio.client->lpVtbl->GetBufferSize(
			d_audio.client,
			&num_frames
		);
		if (FAILED(hr)) {
			fprintf(stderr, "failed to get buffer size\n");
			continue;
		}
		UINT32 padding = 0;
		hr = d_audio.client->lpVtbl->GetCurrentPadding(d_audio.client, &padding);
		if (FAILED(hr)) {
			fprintf(stderr, "failed to get padding\n");
			continue;
		}
        num_frames -= padding;
		if (num_frames <= 0) {
			continue;
		}
		BYTE* data;
		hr = d_audio.render_client->lpVtbl->GetBuffer(
			d_audio.render_client,
			num_frames,
			&data
		);
		if (FAILED(hr)) {
			fprintf(stderr, "failed to get buffer\n");
			continue;
		}
		float* buf = (float*)data;
		for (int i = 0; i < num_frames; i++) {
			buf[i] = d_audio_next();
		}
		hr = d_audio.render_client->lpVtbl->ReleaseBuffer(
			d_audio.render_client,
			num_frames,
			0
		);
		if (FAILED(hr)) {
			fprintf(stderr, "failed to release buffer\n");
			continue;
		}
	}
	return 0;
}


static const CLSID D_CLSID_IMMDeviceEnumerator = { 0xbcde0395, 0xe52f, 0x467c, { 0x8e, 0x3d, 0xc4, 0x57, 0x92, 0x91, 0x69, 0x2e } };
static const IID D_IID_IMMDeviceEnumerator = { 0xa95664d2, 0x9614, 0x4f35, { 0xa7, 0x46, 0xde, 0x8d, 0xb6, 0x36, 0x17, 0xe6 } };
static const IID D_IID_IAudioClient = { 0x1cb9ad4c, 0xdbfa, 0x4c32, { 0xb1, 0x78, 0xc2, 0xf5, 0x68, 0xa7, 0x03, 0xb2 } };
static const IID D_IID_IAudioRenderClient = { 0xf294acfc, 0x3146, 0x4483, { 0xa7, 0xbf, 0xad, 0xdc, 0xa7, 0xc2, 0x60, 0xe2 } };

static void d_wasapi_init(void) {

	HRESULT hr;

	hr = CoInitializeEx(0, COINIT_MULTITHREADED);

	if (FAILED(hr)) {
		fprintf(stderr, "failed to initialize COM\n");
		return;
	}

	hr = CoCreateInstance(
		&D_CLSID_IMMDeviceEnumerator,
		NULL,
		CLSCTX_ALL,
		&D_IID_IMMDeviceEnumerator,
		(void**)&d_audio.device_enumerator
	);

	if (FAILED(hr)) {
		fprintf(stderr, "failed to create device enumerator\n");
		return;
	}

	hr = d_audio.device_enumerator->lpVtbl->GetDefaultAudioEndpoint(
		d_audio.device_enumerator,
		eRender,
		eConsole,
		&d_audio.device
	);

	if (FAILED(hr)) {
		fprintf(stderr, "failed to get default audio device\n");
		return;
	}

	hr = d_audio.device->lpVtbl->Activate(
		d_audio.device,
		&D_IID_IAudioClient,
		CLSCTX_ALL,
		NULL,
		(void**)&d_audio.client
	);

	if (FAILED(hr)) {
		fprintf(stderr, "failed to get activate audio client\n");
		return;
	}

	WAVEFORMATEX fmt;
	memset(&fmt, 0, sizeof(WAVEFORMATEX));
	fmt.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
	fmt.nChannels = D_NUM_CHANNELS;
	fmt.nSamplesPerSec = D_SAMPLE_RATE;
	fmt.wBitsPerSample = 32;
	fmt.nBlockAlign = (fmt.nChannels * fmt.wBitsPerSample) / 8;
	fmt.nAvgBytesPerSec = fmt.nSamplesPerSec * fmt.nBlockAlign;

	d_audio.event = CreateEvent(NULL, FALSE, FALSE, NULL);

    REFERENCE_TIME buf_duration =
		(double)D_BUFFER_FRAMES / ((double)D_SAMPLE_RATE * (1.0 / 10000000.0));

	hr = d_audio.client->lpVtbl->Initialize(
		d_audio.client,
		AUDCLNT_SHAREMODE_SHARED,
		AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
		buf_duration,
		0,
		&fmt,
		NULL
	);

	if (FAILED(hr)) {
		fprintf(stderr, "failed to initialize audio client\n");
		return;
	}

	hr = d_audio.client->lpVtbl->SetEventHandle(d_audio.client, d_audio.event);

	if (FAILED(hr)) {
		fprintf(stderr, "failed to set event handle\n");
		return;
	}

	hr = d_audio.client->lpVtbl->GetService(
		d_audio.client,
		&D_IID_IAudioRenderClient,
		(void**)&d_audio.render_client
	);

	if (FAILED(hr)) {
		fprintf(stderr, "failed to get render client\n");
		return;
	}

	hr = d_audio.client->lpVtbl->Start(d_audio.client);

	if (FAILED(hr)) {
		fprintf(stderr, "failed to start audio client\n");
		return;
	}

	d_audio.thread = CreateThread(NULL, 0, d_wasapi_thread, NULL, 0, NULL);

}

static void d_wasapi_dispose(void) {
	CloseHandle(d_audio.event);
	CloseHandle(d_audio.thread);
	d_audio.render_client->lpVtbl->Release(d_audio.render_client);
	d_audio.client->lpVtbl->Release(d_audio.client);
	d_audio.device->lpVtbl->Release(d_audio.device);
	d_audio.device_enumerator->lpVtbl->Release(d_audio.device_enumerator);
	CoUninitialize();
}

#endif // D_WASAPI

#if defined(D_WEBAUDIO)

EMSCRIPTEN_KEEPALIVE float d_cjs_audio_next(void) {
	return d_audio_next();
}

EM_JS(void, d_js_webaudio_init, (
	int sample_rate,
	int num_channels,
	int buf_size
), {

	window.dAudio = {};

	dAudio.ctx = new AudioContext({
		sampleRate: sample_rate,
	});

	// TODO: try AudioWorklet?
	const scriptNode = dAudio.ctx.createScriptProcessor(buf_size, 0, num_channels);

	scriptNode.addEventListener("audioprocess", (e) => {
		const output = e.outputBuffer.getChannelData(0);
		for (let i = 0; i < buf_size; i++) {
			output[i] = _d_cjs_audio_next();
		}
	});

	scriptNode.connect(dAudio.ctx.destination);
	dAudio.ctx.resume();
	document.addEventListener("click", () => dAudio.ctx.resume(), { once: true });
	document.addEventListener("keydown", () => dAudio.ctx.resume(), { once: true });
	document.addEventListener("touchend", () => dAudio.ctx.resume(), { once: true });

})

EM_JS(void, d_js_webaudio_dispose, (void), {
	dAudio.ctx.close();
})

static void d_webaudio_init(void) {
	d_js_webaudio_init(D_SAMPLE_RATE, D_NUM_CHANNELS, D_BUFFER_FRAMES);
}

static void d_webaudio_dispose(void) {
	d_js_webaudio_dispose();
}

#endif

#if defined(D_ALSA)

static void d_alsa_init(void) {
	// snd_pcm_t* dev = NULL;
	// snd_pcm_open(dev, "default", SND_PCM_STREAM_PLAYBACK, 0);
	// TODO
}

static void d_alsa_dispose(void) {
	// TODO
}

#endif // D_ALSA

void d_audio_init(d_audio_desc desc) {
	d_audio.volume = 1.0;
	d_audio.user_stream = desc.stream;
#if defined(D_COREAUDIO)
	d_ca_init();
#elif defined(D_WEBAUDIO)
	d_webaudio_init();
#elif defined(D_WASAPI)
	d_wasapi_init();
#elif defined(D_ALSA)
	d_alsa_init();
#endif
}

void d_audio_dispose(void) {
#if defined(D_COREAUDIO)
	d_ca_dispose();
#elif defined(D_WEBAUDIO)
	d_webaudio_dispose();
#elif defined(D_WASAPI)
	d_wasapi_dispose();
#elif defined(D_ALSA)
	d_alsa_dispose();
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

d_sound d_sound_clone(d_sound* snd) {
	return d_sound_new(snd->frames, snd->num_frames);
}

d_sound d_sound_stereo_to_mono(d_sound* snd) {
	d_sound snd2 = {0};
	snd2.num_frames = snd->num_frames / 2;
	snd2.frames = malloc(snd2.num_frames * sizeof(short));
	for (int i = 0; i < snd->num_frames; i += 2) {
		snd2.frames[i / 2] = (snd->frames[i] + snd->frames[i + 1]) / 2;
	}
	return snd2;
}

d_sound d_sound_resample(d_sound* src, int input_rate, int output_rate) {
	float ratio = (float) input_rate / output_rate;
	d_sound snd2 = {0};
	snd2.num_frames = src->num_frames / ratio;
	snd2.frames = malloc(snd2.num_frames * sizeof(short));
	for (int i = 0; i < snd2.num_frames; i++) {
		// find the corresponding position in the input buffer
		float src_pos = i * ratio;
		int index = (int)src_pos;
		float frac = src_pos - index;
		// linear interpolation with boundary check
		int interp_val;
		if (index + 1 < src->num_frames) {
			// interpolate between two neighboring samples
			interp_val = (int)((1.0 - frac) * src->frames[index] + frac * src->frames[index + 1]);
		} else {
			// use the last sample if we're at the end of the input
			interp_val = src->frames[index];
		}
		if (interp_val > SHRT_MAX) interp_val = SHRT_MAX;
		if (interp_val < SHRT_MIN) interp_val = SHRT_MIN;
		snd2.frames[i] = (short)interp_val;
	}
	return snd2;
}

static uint8_t OGG_SIG[] = { 0x4f, 0x67, 0x67, 0x53 };
static uint8_t WAV_SIG[] = { 0x52, 0x49, 0x46, 0x46 };

d_sound d_sound_parse(uint8_t* bytes, size_t size) {
	if (memcmp(bytes, OGG_SIG, sizeof(OGG_SIG)) == 0) {
#ifdef STB_VORBIS_IMPLEMENTATION
		int num_channels, sample_rate;
		short* frames;
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
		d_sound snd = {
			.num_frames = num_frames * num_channels,
			.frames = frames,
		};
		if (num_channels == 2 && D_NUM_CHANNELS == 1) {
			d_sound snd2 = d_sound_stereo_to_mono(&snd);
			d_sound_free(&snd);
			snd = snd2;
		}
		if (sample_rate != D_SAMPLE_RATE) {
			d_sound snd2 = d_sound_resample(&snd, sample_rate, D_SAMPLE_RATE);
			d_sound_free(&snd);
			snd = snd2;
		}
		return snd;
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
	int pos = d_clampi(time * D_SAMPLE_RATE, 0, snd->num_frames - 1);
	return (float)snd->frames[pos] / SHRT_MAX;
}

float d_sound_duration(d_sound* snd) {
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
		.speed = 1.0,
	});
}

d_playback* d_play_ex(d_sound* snd, d_play_opts opts) {
	if (d_audio.num_playbacks >= D_MAX_PLAYBACKS) {
		return NULL;
	}
	int pos = d_clampi((int)(opts.time * D_SAMPLE_RATE), 0, snd->num_frames - 1);
	d_playback src = (d_playback) {
		.src = snd,
		.pos = pos,
		.loop = opts.loop,
		.paused = opts.paused,
		.volume = opts.volume,
		.speed = opts.speed,
		.done = false,
	};
	d_audio.playbacks[d_audio.num_playbacks] = src;
	return &d_audio.playbacks[d_audio.num_playbacks++];
}

void d_playback_seek(d_playback* pb, float time) {
	pb->pos = d_clampi(time * D_SAMPLE_RATE, 0, pb->src->num_frames - 1);
}

void d_playback_seek_by(d_playback* pb, float time) {
	d_playback_seek(pb, d_playback_time(pb) + time);
}

float d_playback_time(d_playback* pb) {
	return (float)pb->pos / (float)D_SAMPLE_RATE;
}

#endif // #ifndef D_AUDIO_IMPL_ONCE
#endif // #ifdef D_AUDIO_IMPL
