// wengwengweng

#ifndef D_APP_H
#define D_APP_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "math.h"
#include "event.h"

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

