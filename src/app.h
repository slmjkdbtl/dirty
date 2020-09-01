// wengwengweng

#ifndef D_APP_H
#define D_APP_H

#include <SDL2/SDL.h>
#include <stdbool.h>

#include "math.h"
#include "event.h"
#include "gfx.h"

typedef enum {
	D_BTN_IDLE = 0,
	D_BTN_PRESSED,
	D_BTN_RELEASED,
	D_BTN_DOWN,
} d_btn_state;

typedef struct {
	SDL_GLContext gl;
	SDL_Window* window;
	bool quit;
	float time;
	float dt;
	int width;
	int height;
	vec2 mouse_pos;
	vec2 mouse_dpos;
	d_btn_state key_states[128];
	d_btn_state mouse_states[4];
	d_mesh tri_mesh;
	d_program default_prog;
	d_program* cur_prog;
	mat4 transform;
	mat4 t_stack[16];
	size_t t_stack_cnt;
} d_ctx;

extern d_ctx d;

void d_init(const char*, int, int);
void d_run(void (*)(void));
void d_quit();
float d_time();
float d_dt();
bool d_key_pressed(d_key);
bool d_key_released(d_key);
bool d_key_down(d_key);
bool d_mouse_pressed(d_mouse);
bool d_mouse_released(d_mouse);
bool d_mouse_down(d_mouse);
int d_width();
int d_height();
vec2 d_mouse_pos();
vec2 d_mouse_dpos();

#endif

