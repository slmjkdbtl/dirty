// wengwengweng

#ifndef D_SAPP

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <SDL2/SDL.h>

#include <dirty/dirty.h>

#ifdef D_WEB
#include <emscripten.h>
#endif

#include "gfx.h"
#include "audio.h"
#include "ui.h"

#define NUM_TOUCHES 8
#define INPUT_BUF_LEN 32

typedef enum {
	D_BTN_IDLE,
	D_BTN_PRESSED,
	D_BTN_RPRESSED,
	D_BTN_RELEASED,
	D_BTN_DOWN,
} d_btn;

typedef struct {
	vec2 pos;
	vec2 dpos;
	d_btn state;
} d_touch_state;

typedef struct {
	d_desc desc;
	SDL_GLContext gl;
	SDL_Window *window;
	bool quit;
	float time;
	float dt;
	int width;
	int height;
	vec2 mouse_pos;
	vec2 mouse_dpos;
	vec2 wheel;
	d_btn key_states[_D_NUM_KEYS];
	d_btn mouse_states[_D_NUM_MOUSE];
	d_touch_state touches[NUM_TOUCHES];
	bool resized;
	char char_input;
} d_app_t;

static d_app_t d_app;

static d_mouse sdl_mouse_to_d(int btn) {
	switch (btn) {
		case SDL_BUTTON_LEFT: return D_MOUSE_LEFT;
		case SDL_BUTTON_RIGHT: return D_MOUSE_RIGHT;
		case SDL_BUTTON_MIDDLE: return D_MOUSE_MIDDLE;
		default: return D_MOUSE_NONE;
	}
	return D_MOUSE_NONE;
}

static d_key sdl_key_to_d(SDL_Scancode code) {
	switch (code) {
		case SDL_SCANCODE_A: return D_KEY_A;
		case SDL_SCANCODE_B: return D_KEY_B;
		case SDL_SCANCODE_C: return D_KEY_C;
		case SDL_SCANCODE_D: return D_KEY_D;
		case SDL_SCANCODE_E: return D_KEY_E;
		case SDL_SCANCODE_F: return D_KEY_F;
		case SDL_SCANCODE_G: return D_KEY_G;
		case SDL_SCANCODE_H: return D_KEY_H;
		case SDL_SCANCODE_I: return D_KEY_I;
		case SDL_SCANCODE_J: return D_KEY_J;
		case SDL_SCANCODE_K: return D_KEY_K;
		case SDL_SCANCODE_L: return D_KEY_L;
		case SDL_SCANCODE_M: return D_KEY_M;
		case SDL_SCANCODE_N: return D_KEY_N;
		case SDL_SCANCODE_O: return D_KEY_O;
		case SDL_SCANCODE_P: return D_KEY_P;
		case SDL_SCANCODE_Q: return D_KEY_Q;
		case SDL_SCANCODE_R: return D_KEY_R;
		case SDL_SCANCODE_S: return D_KEY_S;
		case SDL_SCANCODE_T: return D_KEY_T;
		case SDL_SCANCODE_U: return D_KEY_U;
		case SDL_SCANCODE_V: return D_KEY_V;
		case SDL_SCANCODE_W: return D_KEY_W;
		case SDL_SCANCODE_X: return D_KEY_X;
		case SDL_SCANCODE_Y: return D_KEY_Y;
		case SDL_SCANCODE_Z: return D_KEY_Z;
		case SDL_SCANCODE_1: return D_KEY_1;
		case SDL_SCANCODE_2: return D_KEY_2;
		case SDL_SCANCODE_3: return D_KEY_3;
		case SDL_SCANCODE_4: return D_KEY_4;
		case SDL_SCANCODE_5: return D_KEY_5;
		case SDL_SCANCODE_6: return D_KEY_6;
		case SDL_SCANCODE_7: return D_KEY_7;
		case SDL_SCANCODE_8: return D_KEY_8;
		case SDL_SCANCODE_9: return D_KEY_9;
		case SDL_SCANCODE_0: return D_KEY_0;
		case SDL_SCANCODE_MINUS: return D_KEY_MINUS;
		case SDL_SCANCODE_EQUALS: return D_KEY_EQUAL;
		case SDL_SCANCODE_SPACE: return D_KEY_SPACE;
		case SDL_SCANCODE_COMMA: return D_KEY_COMMA;
		case SDL_SCANCODE_PERIOD: return D_KEY_PERIOD;
		case SDL_SCANCODE_SLASH: return D_KEY_SLASH;
		case SDL_SCANCODE_LEFTBRACKET: return D_KEY_LBRACKET;
		case SDL_SCANCODE_RIGHTBRACKET: return D_KEY_RBRACKET;
		case SDL_SCANCODE_BACKSLASH: return D_KEY_BACKSLASH;
		case SDL_SCANCODE_SEMICOLON: return D_KEY_SEMICOLON;
		case SDL_SCANCODE_RETURN: return D_KEY_ENTER;
		case SDL_SCANCODE_ESCAPE: return D_KEY_ESC;
		case SDL_SCANCODE_BACKSPACE: return D_KEY_BACKSPACE;
		case SDL_SCANCODE_TAB: return D_KEY_TAB;
		case SDL_SCANCODE_APOSTROPHE: return D_KEY_QUOTE;
		case SDL_SCANCODE_GRAVE: return D_KEY_BACKQUOTE;
		case SDL_SCANCODE_F1: return D_KEY_F1;
		case SDL_SCANCODE_F2: return D_KEY_F2;
		case SDL_SCANCODE_F3: return D_KEY_F3;
		case SDL_SCANCODE_F4: return D_KEY_F4;
		case SDL_SCANCODE_F5: return D_KEY_F5;
		case SDL_SCANCODE_F6: return D_KEY_F6;
		case SDL_SCANCODE_F7: return D_KEY_F7;
		case SDL_SCANCODE_F8: return D_KEY_F8;
		case SDL_SCANCODE_F9: return D_KEY_F9;
		case SDL_SCANCODE_F10: return D_KEY_F10;
		case SDL_SCANCODE_F11: return D_KEY_F11;
		case SDL_SCANCODE_F12: return D_KEY_F12;
		case SDL_SCANCODE_RIGHT: return D_KEY_RIGHT;
		case SDL_SCANCODE_LEFT: return D_KEY_LEFT;
		case SDL_SCANCODE_DOWN: return D_KEY_DOWN;
		case SDL_SCANCODE_UP: return D_KEY_UP;
		case SDL_SCANCODE_LCTRL: return D_KEY_LCTRL;
		case SDL_SCANCODE_RCTRL: return D_KEY_RCTRL;
		case SDL_SCANCODE_LALT: return D_KEY_LALT;
		case SDL_SCANCODE_RALT: return D_KEY_RALT;
		case SDL_SCANCODE_LGUI: return D_KEY_LMETA;
		case SDL_SCANCODE_RGUI: return D_KEY_RMETA;
		case SDL_SCANCODE_LSHIFT: return D_KEY_LSHIFT;
		case SDL_SCANCODE_RSHIFT: return D_KEY_RSHIFT;
		default: return D_KEY_NONE;
	}
	return D_KEY_NONE;
}

void d_process_btn(d_btn *b) {
	if (*b == D_BTN_PRESSED || *b == D_BTN_RPRESSED) {
		*b = D_BTN_DOWN;
	} else if (*b == D_BTN_RELEASED) {
		*b = D_BTN_IDLE;
	}
}

static void d_frame() {

	d_clear();

	if (d_app.desc.frame) {
		d_app.desc.frame();
	}

	if (d_app.desc.frame_u) {
		d_app.desc.frame_u(d_app.desc.udata);
	}

	d_gfx_frame_end();
	SDL_GL_SwapWindow(d_app.window);

	SDL_Event event;

	// time
	int time = SDL_GetTicks();

	d_app.dt = time / 1000.0 - d_app.time;
	d_app.time = time / 1000.0;

	// reset input states
	for (int i = 0; i < _D_NUM_KEYS; i++) {
		d_process_btn(&d_app.key_states[i]);
	}

	for (int i = 0; i < _D_NUM_MOUSE; i++) {
		d_process_btn(&d_app.mouse_states[i]);
	}

	for (int i = 0; i < NUM_TOUCHES; i++) {
		d_process_btn(&d_app.touches[i].state);
	}

	d_app.wheel.x = 0.0;
	d_app.wheel.y = 0.0;
	d_app.resized = false;
	d_app.char_input = 0;

	// deal with inputs
	while (SDL_PollEvent(&event)) {

		d_key key = sdl_key_to_d(event.key.keysym.scancode);
		d_mouse mouse = sdl_mouse_to_d(event.button.button);

		switch (event.type) {
			case SDL_QUIT:
				d_quit();
				break;
			case SDL_KEYDOWN:
				if (d_key_down(key)) {
					d_app.key_states[key] = D_BTN_RPRESSED;
				} else {
					d_app.key_states[key] = D_BTN_PRESSED;
				}
				break;
			case SDL_KEYUP:
				d_app.key_states[key] = D_BTN_RELEASED;
				break;
			case SDL_MOUSEBUTTONDOWN:
				d_app.mouse_states[mouse] = D_BTN_PRESSED;
				break;
			case SDL_MOUSEBUTTONUP:
				d_app.mouse_states[mouse] = D_BTN_RELEASED;
				break;
			case SDL_MOUSEMOTION:
				break;
			case SDL_MOUSEWHEEL:
				d_app.wheel.x = -event.wheel.x;
				d_app.wheel.y = -event.wheel.y;
				break;
			case SDL_TEXTINPUT:
				d_app.char_input = event.text.text[0];
				break;
			case SDL_FINGERDOWN:
				break;
			case SDL_FINGERUP:
				break;
			case SDL_FINGERMOTION:
				break;
			case SDL_WINDOWEVENT:
				switch (event.window.event) {
					case SDL_WINDOWEVENT_RESIZED:
						d_app.resized = true;
						break;
				}
				break;
		}

	}

	SDL_GetWindowSize(d_app.window, &d_app.width, &d_app.height);

	int mx, my, dx, dy;

	SDL_GetMouseState(&mx, &my);
	SDL_GetRelativeMouseState(&dx, &dy);

	d_app.mouse_pos.x = (float)mx - d_app.width / 2.0;
	d_app.mouse_pos.y = d_app.height / 2.0 - (float)my;
	d_app.mouse_dpos.x = (float)dx;
	d_app.mouse_dpos.y = -(float)dy;

}

void d_run(d_desc desc) {

	desc.title = desc.title ? desc.title : "";
	desc.width = desc.width ? desc.width : 640;
	desc.height = desc.height ? desc.height : 480;

	d_app.desc = desc;

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#ifdef GLES
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#endif

	d_app.window = SDL_CreateWindow(
		desc.title,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		desc.width,
		desc.height,
		SDL_WINDOW_OPENGL
	);

	d_app.gl = SDL_GL_CreateContext(d_app.window);
	SDL_GL_SetSwapInterval(1);
    SDL_GL_MakeCurrent(d_app.window, d_app.gl);
	SDL_GetWindowSize(d_app.window, &d_app.width, &d_app.height);

	d_gfx_init();
	d_audio_init();
	d_ui_init();

	if (d_app.desc.init) {
		d_app.desc.init();
	}

	if (d_app.desc.init_u) {
		d_app.desc.init_u(d_app.desc.udata);
	}

#ifdef D_WEB
	emscripten_set_main_loop(d_frame, 0, 0);
#else

	while (!d_app.quit) {
		d_frame();
	}

	d_audio_cleanup();
	SDL_GL_DeleteContext(d_app.gl);
	SDL_DestroyWindow(d_app.window);
	SDL_Quit();

	if (d_app.desc.cleanup) {
		d_app.desc.cleanup();
	}

	if (d_app.desc.cleanup_u) {
		d_app.desc.cleanup_u(d_app.desc.udata);
	}

#endif

}

void d_quit() {
	d_app.quit = true;
}

void d_fail(const char *fmt, ...) {
	d_quit();
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fflush(stdout);
	fflush(stderr);
	exit(EXIT_FAILURE);
}

void d_err(const char *fmt, ...) {

	va_list args;

	va_start(args, fmt);
	int size = vsnprintf(NULL, 0, fmt, args) + 1;
	va_end(args);

	char *msg = malloc(size);

	va_start(args, fmt);
	vsnprintf(msg, size, fmt, args);
	va_end(args);

	if (d_app.desc.err) {
		d_app.desc.err(msg);
	}

	if (d_app.desc.err_u) {
		d_app.desc.err_u(d_app.desc.udata, msg);
	}

	free(msg);

}

void d_assert(bool test, const char *fmt, ...) {
	if (!test) {
		d_quit();
		va_list args;
		va_start(args, fmt);
		vfprintf(stderr, fmt, args);
		va_end(args);
		fflush(stdout);
		fflush(stderr);
		exit(EXIT_FAILURE);
	}
}

float d_time() {
	return d_app.time;
}

float d_dt() {
	return d_app.dt;
}

void d_vsync(bool b) {
	if (b) {
		SDL_GL_SetSwapInterval(1);
	} else {
		SDL_GL_SetSwapInterval(0);
	}
}

void d_set_fullscreen(bool b) {
	if (b) {
		SDL_SetWindowFullscreen(d_app.window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	} else {
		SDL_SetWindowFullscreen(d_app.window, 0);
	}
}

bool d_fullscreen() {
	int flags = SDL_GetWindowFlags(d_app.window);
	return flags & SDL_WINDOW_FULLSCREEN;
}

void d_set_mouse_relative(bool b) {
	SDL_SetRelativeMouseMode(b);
}

bool d_mouse_relative() {
	return SDL_GetRelativeMouseMode();
}

void d_set_mouse_hidden(bool b) {
	SDL_ShowCursor(b);
}

bool d_mouse_hidden() {
	return SDL_ShowCursor(SDL_QUERY);
}

void d_set_title(const char *title) {
	SDL_SetWindowTitle(d_app.window, title);
}

const char *d_title() {
	return SDL_GetWindowTitle(d_app.window);
}

// TODO
bool d_key_pressed(d_key k) {
	return d_app.key_states[k] == D_BTN_PRESSED;
}

bool d_key_rpressed(d_key k) {
	return d_app.key_states[k] == D_BTN_PRESSED || d_app.key_states[k] == D_BTN_RPRESSED;
}

bool d_key_released(d_key k) {
	return d_app.key_states[k] == D_BTN_RELEASED;
}

bool d_key_down(d_key k) {
	return d_app.key_states[k] == D_BTN_DOWN || d_app.key_states[k] == D_BTN_PRESSED;
}

bool d_key_mod(d_kmod kmod) {
	switch (kmod) {
		case D_KMOD_ALT: return d_key_down(D_KEY_LALT) || d_key_down(D_KEY_RALT);
		case D_KMOD_META: return d_key_down(D_KEY_LMETA) || d_key_down(D_KEY_RMETA);
		case D_KMOD_CTRL: return d_key_down(D_KEY_LCTRL) || d_key_down(D_KEY_RCTRL);
		case D_KMOD_SHIFT: return d_key_down(D_KEY_LSHIFT) || d_key_down(D_KEY_RSHIFT);
		default: return false;
	}
	return false;
}

bool d_mouse_pressed(d_mouse k) {
	return d_app.mouse_states[k] == D_BTN_PRESSED;
}

bool d_mouse_released(d_mouse k) {
	return d_app.mouse_states[k] == D_BTN_RELEASED;
}

bool d_mouse_down(d_mouse k) {
	return d_app.mouse_states[k] == D_BTN_DOWN || d_app.mouse_states[k] == D_BTN_PRESSED;
}

int d_width() {
	return d_app.width;
}

int d_height() {
	return d_app.height;
}

vec2 d_mouse_pos() {
	return d_app.mouse_pos;
}

vec2 d_mouse_dpos() {
	return d_app.mouse_dpos;
}

bool d_mouse_moved() {
	return d_app.mouse_dpos.x != 0.0 || d_app.mouse_dpos.y != 0.0;
}

bool d_touch_pressed(d_touch t) {
	d_assert(t < NUM_TOUCHES, "touch not found: %d\n", t);
	return d_app.touches[t].state == D_BTN_PRESSED;
}

bool d_touch_released(d_touch t) {
	d_assert(t < NUM_TOUCHES, "touch not found: %d\n", t);
	return d_app.touches[t].state == D_BTN_RELEASED;
}

bool d_touch_moved(d_touch t) {
	d_assert(t < NUM_TOUCHES, "touch not found: %d\n", t);
	return d_app.touches[t].dpos.x != 0.0 || d_app.touches[t].dpos.x != 0.0;
}

vec2 d_touch_pos(d_touch t) {
	d_assert(t < NUM_TOUCHES, "touch not found: %d\n", t);
	return d_app.touches[t].pos;
}

vec2 d_touch_dpos(d_touch t) {
	d_assert(t < NUM_TOUCHES, "touch not found: %d\n", t);
	return d_app.touches[t].dpos;
}

bool d_resized() {
	return d_app.resized;
}

bool d_scrolled() {
	return d_app.wheel.x != 0.0 || d_app.wheel.y != 0.0;
}

vec2 d_wheel() {
	return d_app.wheel;
}

char d_input() {
	return d_app.char_input;
}

#endif

