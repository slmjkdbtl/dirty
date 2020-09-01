// wengwengweng

#include <stdio.h>

#include "app.h"
#include "gl.h"

d_ctx d;

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

void d_init(const char* title, int width, int height) {

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

	d.window = SDL_CreateWindow(
		title,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width,
		height,
		SDL_WINDOW_SHOWN
	);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	d.gl = SDL_GL_CreateContext(d.window);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	const GLubyte* gl_ver = glGetString(GL_VERSION);
	printf("%s\n", gl_ver);

	d_vertex verts[] = {
		{
			.pos = { 0.0, 120.0, 0.0, },
			.normal = { 0.0, 0.0, 1.0 },
			.uv = { 0.0, 0.0, },
			.color = { 1.0, 0.0, 0.0, 1.0, }
		},
		{
			.pos = { -160.0, -120.0, 0.0, },
			.normal = { 0.0, 0.0, 1.0 },
			.uv = { 0.0, 0.0, },
			.color = { 0.0, 1.0, 0.0, 1.0, }
		},
		{
			.pos = { 160.0, -120.0, 0.0, },
			.normal = { 0.0, 0.0, 1.0 },
			.uv = { 0.0, 0.0, },
			.color = { 0.0, 0.0, 1.0, 1.0, }
		},
	};

	unsigned int indices[] = {
		0, 1, 2,
	};

	SDL_GL_SwapWindow(d.window);
	SDL_GetWindowSize(d.window, &d.width, &d.height);

	d.tri_mesh = d_make_mesh(verts, sizeof(verts), indices, sizeof(indices));
	d.default_prog = d_make_program(d_vert_default, d_frag_default);
	d.cur_prog = &d.default_prog;
	d.empty_tex = d_make_tex((unsigned char[]){255, 255, 255, 255}, 1, 1);
	d.tex_slots[0] = &d.empty_tex;
	d.transform = make_mat4();
	d.view = make_mat4();
	d.proj = mat4_ortho(d.width, d.height, -1024.0, 1024.0);

}

void d_run(void (*f)(void)) {

	if (!f) {
		fprintf(stderr, "invalid run func");
		return d_quit();
	}

	SDL_Event event;

	while (!d.quit) {

		int time = SDL_GetTicks();

		d.dt = time / 1000.0 - d.time;
		d.time = time / 1000.0;

		for (int i = 0; i < 128; i++) {
			if (d.key_states[i] == D_BTN_PRESSED) {
				d.key_states[i] = D_BTN_DOWN;
			} else if (d.key_states[i] == D_BTN_RELEASED) {
				d.key_states[i] = D_BTN_IDLE;
			}
		}

		for (int i = 0; i < 4; i++) {
			if (d.mouse_states[i] == D_BTN_PRESSED) {
				d.mouse_states[i] = D_BTN_DOWN;
			} else if (d.mouse_states[i] == D_BTN_RELEASED) {
				d.mouse_states[i] = D_BTN_IDLE;
			}
		}

		int mx, my;

		SDL_GetWindowSize(d.window, &d.width, &d.height);
		SDL_GetMouseState(&mx, &my);

		mx = (float)mx;
		my = (float)my;

		if (d.mouse_pos.x != 0.0 || d.mouse_pos.y != 0.0) {
			d.mouse_dpos.x = mx - d.mouse_pos.x;
			d.mouse_dpos.y = mx - d.mouse_pos.y;
		}

		d.mouse_pos.x = mx;
		d.mouse_pos.y = my;
		d.transform = make_mat4();

		while (SDL_PollEvent(&event)) {

			d_key key = sdl_key_to_d(event.key.keysym.scancode);
			d_mouse mouse = sdl_mouse_to_d(event.button.button);

			switch (event.type) {
				case SDL_QUIT:
					d.quit = true;
					break;
				case SDL_KEYDOWN:
					d.key_states[key] = D_BTN_PRESSED;
					break;
				case SDL_KEYUP:
					d.key_states[key] = D_BTN_RELEASED;
					break;
				case SDL_MOUSEBUTTONDOWN:
					d.mouse_states[mouse] = D_BTN_PRESSED;
					break;
				case SDL_MOUSEBUTTONUP:
					d.mouse_states[mouse] = D_BTN_RELEASED;
					break;
			}

		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		d_draw(&d.tri_mesh, &d.default_prog);
		f();
		SDL_GL_SwapWindow(d.window);

	}

	SDL_GL_DeleteContext(d.gl);
	SDL_DestroyWindow(d.window);
	SDL_Quit();

}

void d_quit() {
	d.quit = true;
}

float d_time() {
	return d.time;
}

float d_dt() {
	return d.dt;
}

bool d_key_pressed(d_key k) {
	return d.key_states[k] == D_BTN_PRESSED;
}

bool d_key_released(d_key k) {
	return d.key_states[k] == D_BTN_RELEASED;
}

bool d_key_down(d_key k) {
	return d.key_states[k] == D_BTN_DOWN || d.key_states[k] == D_BTN_PRESSED;
}

bool d_mouse_pressed(d_mouse k) {
	return d.mouse_states[k] == D_BTN_PRESSED;
}

bool d_mouse_released(d_mouse k) {
	return d.mouse_states[k] == D_BTN_RELEASED;
}

bool d_mouse_down(d_mouse k) {
	return d.mouse_states[k] == D_BTN_DOWN || d.mouse_states[k] == D_BTN_PRESSED;
}

int d_width() {
	return d.width;
}

int d_height() {
	return d.height;
}

vec2 d_mouse_pos() {
	return d.mouse_pos;
}

vec2 d_mouse_dpos() {
	return d.mouse_dpos;
}

