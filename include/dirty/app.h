// wengwengweng

#ifndef D_APP_H
#define D_APP_H

#include <stdbool.h>
#include "math.h"

typedef enum {
	D_KEY_NONE,
	D_KEY_Q,
	D_KEY_W,
	D_KEY_E,
	D_KEY_R,
	D_KEY_T,
	D_KEY_Y,
	D_KEY_U,
	D_KEY_I,
	D_KEY_O,
	D_KEY_P,
	D_KEY_A,
	D_KEY_S,
	D_KEY_D,
	D_KEY_F,
	D_KEY_G,
	D_KEY_H,
	D_KEY_J,
	D_KEY_K,
	D_KEY_L,
	D_KEY_Z,
	D_KEY_X,
	D_KEY_C,
	D_KEY_V,
	D_KEY_B,
	D_KEY_N,
	D_KEY_M,
	D_KEY_1,
	D_KEY_2,
	D_KEY_3,
	D_KEY_4,
	D_KEY_5,
	D_KEY_6,
	D_KEY_7,
	D_KEY_8,
	D_KEY_9,
	D_KEY_0,
	D_KEY_F1,
	D_KEY_F2,
	D_KEY_F3,
	D_KEY_F4,
	D_KEY_F5,
	D_KEY_F6,
	D_KEY_F7,
	D_KEY_F8,
	D_KEY_F9,
	D_KEY_F10,
	D_KEY_F11,
	D_KEY_F12,
	D_KEY_MINUS,
	D_KEY_EQUAL,
	D_KEY_COMMA,
	D_KEY_PERIOD,
	D_KEY_BACKQUOTE,
	D_KEY_SLASH,
	D_KEY_BACKSLASH,
	D_KEY_SEMICOLON,
	D_KEY_QUOTE,
	D_KEY_UP,
	D_KEY_DOWN,
	D_KEY_LEFT,
	D_KEY_RIGHT,
	D_KEY_ESC,
	D_KEY_TAB,
	D_KEY_SPACE,
	D_KEY_BACKSPACE,
	D_KEY_ENTER,
	D_KEY_LBRACKET,
	D_KEY_RBRACKET,
	D_KEY_LSHIFT,
	D_KEY_RSHIFT,
	D_KEY_LALT,
	D_KEY_RALT,
	D_KEY_LMETA,
	D_KEY_RMETA,
	D_KEY_LCTRL,
	D_KEY_RCTRL,
} d_key;

typedef enum {
	D_MOUSE_NONE,
	D_MOUSE_LEFT,
	D_MOUSE_RIGHT,
	D_MOUSE_MIDDLE,
} d_mouse;

// lifecycle
void d_init(const char*, int, int);
void d_run(void (*)());
void d_quit();
void d_fail();

// settings / query
void d_set_vsync(bool);
void d_set_fullscreen(bool);
bool d_fullscreen();
void d_set_mouse_relative(bool);
bool d_mouse_relative();
void d_set_mouse_hidden(bool);
bool d_mouse_hidden();
void d_set_title(const char*);
const char* d_title();
int d_width();
int d_height();

// time
float d_time();
float d_dt();

// input
bool d_key_pressed(d_key);
bool d_key_released(d_key);
bool d_key_down(d_key);
bool d_mouse_pressed(d_mouse);
bool d_mouse_released(d_mouse);
bool d_mouse_down(d_mouse);
bool d_mouse_moved();
vec2 d_mouse_pos();
vec2 d_mouse_dpos();
bool d_scrolled();
vec2 d_wheel();
bool d_resized();
const char* d_tinput();

#endif

