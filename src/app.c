// wengwengweng

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>

#include <dirty/dirty.h>

#if defined(D_MACOS) || defined(D_LINUX) || defined(D_WINDOWS)
	#define SOKOL_GLCORE33
#elif defined(D_IOS) || defined(D_ANDROID) || defined(D_WEB)
	#define SOKOL_GLES2
#endif

#define SOKOL_IMPL
#define SOKOL_NO_ENTRY
#include <sokol/sokol_app.h>
#include <sokol/sokol_time.h>

#define NUM_TOUCHES 8

static const char *vs_src =
#ifndef GLES
"#version 120\n"
#endif
"attribute vec2 a_pos;"
"attribute vec2 a_uv;"
"varying vec2 v_uv;"
"void main() {"
	"v_uv = a_uv;"
	"gl_Position = vec4(a_pos, 0.0, 1.0);"
"}"
;

static const char *fs_src =
#ifndef GLES
"#version 120\n"
#else
"precision mediump float;"
#endif
"varying vec2 v_uv;"
"uniform sampler2D u_tex;"
"void main() {"
	"gl_FragColor = texture2D(u_tex, v_uv);"
"}"
;

void d_gfx_init(d_desc *desc);
void d_audio_init(d_desc *desc);
void d_fs_init(d_desc *desc);
void d_gfx_frame_end();
void d_audio_quit();

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
	float time;
	float dt;
	vec2 mouse_pos;
	vec2 mouse_dpos;
	vec2 wheel;
	d_btn key_states[_D_NUM_KEYS];
	d_btn mouse_states[_D_NUM_MOUSE];
	d_touch_state touches[NUM_TOUCHES];
	bool resized;
	char char_input;

	GLuint gl_prog;
	GLuint gl_vbuf;
	GLuint gl_ibuf;
	GLuint gl_tex;

} d_app_t;

static d_app_t d_app;

static d_mouse sapp_mouse_to_d(sapp_mousebutton btn) {
	switch (btn) {
		case SAPP_MOUSEBUTTON_LEFT: return D_MOUSE_LEFT;
		case SAPP_MOUSEBUTTON_RIGHT: return D_MOUSE_RIGHT;
		case SAPP_MOUSEBUTTON_MIDDLE: return D_MOUSE_MIDDLE;
		default: return D_MOUSE_NONE;
	}
	return D_MOUSE_NONE;
}

static d_key sapp_key_to_d(sapp_keycode code) {
	switch (code) {
		case SAPP_KEYCODE_A: return D_KEY_A;
		case SAPP_KEYCODE_B: return D_KEY_B;
		case SAPP_KEYCODE_C: return D_KEY_C;
		case SAPP_KEYCODE_D: return D_KEY_D;
		case SAPP_KEYCODE_E: return D_KEY_E;
		case SAPP_KEYCODE_F: return D_KEY_F;
		case SAPP_KEYCODE_G: return D_KEY_G;
		case SAPP_KEYCODE_H: return D_KEY_H;
		case SAPP_KEYCODE_I: return D_KEY_I;
		case SAPP_KEYCODE_J: return D_KEY_J;
		case SAPP_KEYCODE_K: return D_KEY_K;
		case SAPP_KEYCODE_L: return D_KEY_L;
		case SAPP_KEYCODE_M: return D_KEY_M;
		case SAPP_KEYCODE_N: return D_KEY_N;
		case SAPP_KEYCODE_O: return D_KEY_O;
		case SAPP_KEYCODE_P: return D_KEY_P;
		case SAPP_KEYCODE_Q: return D_KEY_Q;
		case SAPP_KEYCODE_R: return D_KEY_R;
		case SAPP_KEYCODE_S: return D_KEY_S;
		case SAPP_KEYCODE_T: return D_KEY_T;
		case SAPP_KEYCODE_U: return D_KEY_U;
		case SAPP_KEYCODE_V: return D_KEY_V;
		case SAPP_KEYCODE_W: return D_KEY_W;
		case SAPP_KEYCODE_X: return D_KEY_X;
		case SAPP_KEYCODE_Y: return D_KEY_Y;
		case SAPP_KEYCODE_Z: return D_KEY_Z;
		case SAPP_KEYCODE_1: return D_KEY_1;
		case SAPP_KEYCODE_2: return D_KEY_2;
		case SAPP_KEYCODE_3: return D_KEY_3;
		case SAPP_KEYCODE_4: return D_KEY_4;
		case SAPP_KEYCODE_5: return D_KEY_5;
		case SAPP_KEYCODE_6: return D_KEY_6;
		case SAPP_KEYCODE_7: return D_KEY_7;
		case SAPP_KEYCODE_8: return D_KEY_8;
		case SAPP_KEYCODE_9: return D_KEY_9;
		case SAPP_KEYCODE_0: return D_KEY_0;
		case SAPP_KEYCODE_MINUS: return D_KEY_MINUS;
		case SAPP_KEYCODE_EQUAL: return D_KEY_EQUAL;
		case SAPP_KEYCODE_SPACE: return D_KEY_SPACE;
		case SAPP_KEYCODE_COMMA: return D_KEY_COMMA;
		case SAPP_KEYCODE_PERIOD: return D_KEY_PERIOD;
		case SAPP_KEYCODE_SLASH: return D_KEY_SLASH;
		case SAPP_KEYCODE_LEFT_BRACKET: return D_KEY_LBRACKET;
		case SAPP_KEYCODE_RIGHT_BRACKET: return D_KEY_RBRACKET;
		case SAPP_KEYCODE_BACKSLASH: return D_KEY_BACKSLASH;
		case SAPP_KEYCODE_SEMICOLON: return D_KEY_SEMICOLON;
		case SAPP_KEYCODE_ENTER: return D_KEY_ENTER;
		case SAPP_KEYCODE_ESCAPE: return D_KEY_ESC;
		case SAPP_KEYCODE_BACKSPACE: return D_KEY_BACKSPACE;
		case SAPP_KEYCODE_TAB: return D_KEY_TAB;
		case SAPP_KEYCODE_APOSTROPHE: return D_KEY_QUOTE;
		case SAPP_KEYCODE_GRAVE_ACCENT: return D_KEY_BACKQUOTE;
		case SAPP_KEYCODE_F1: return D_KEY_F1;
		case SAPP_KEYCODE_F2: return D_KEY_F2;
		case SAPP_KEYCODE_F3: return D_KEY_F3;
		case SAPP_KEYCODE_F4: return D_KEY_F4;
		case SAPP_KEYCODE_F5: return D_KEY_F5;
		case SAPP_KEYCODE_F6: return D_KEY_F6;
		case SAPP_KEYCODE_F7: return D_KEY_F7;
		case SAPP_KEYCODE_F8: return D_KEY_F8;
		case SAPP_KEYCODE_F9: return D_KEY_F9;
		case SAPP_KEYCODE_F10: return D_KEY_F10;
		case SAPP_KEYCODE_F11: return D_KEY_F11;
		case SAPP_KEYCODE_F12: return D_KEY_F12;
		case SAPP_KEYCODE_RIGHT: return D_KEY_RIGHT;
		case SAPP_KEYCODE_LEFT: return D_KEY_LEFT;
		case SAPP_KEYCODE_DOWN: return D_KEY_DOWN;
		case SAPP_KEYCODE_UP: return D_KEY_UP;
		case SAPP_KEYCODE_LEFT_CONTROL: return D_KEY_LCTRL;
		case SAPP_KEYCODE_RIGHT_CONTROL: return D_KEY_RCTRL;
		case SAPP_KEYCODE_LEFT_ALT: return D_KEY_LALT;
		case SAPP_KEYCODE_RIGHT_ALT: return D_KEY_RALT;
		case SAPP_KEYCODE_LEFT_SUPER: return D_KEY_LMETA;
		case SAPP_KEYCODE_RIGHT_SUPER: return D_KEY_RMETA;
		case SAPP_KEYCODE_LEFT_SHIFT: return D_KEY_LSHIFT;
		case SAPP_KEYCODE_RIGHT_SHIFT: return D_KEY_RSHIFT;
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

static void init() {

	// program
	GLchar info_log[512];
	GLint success = 0;

	// vertex shader
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vs, 1, &vs_src, 0);
	glCompileShader(vs);

	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE) {
		glGetShaderInfoLog(vs, 512, NULL, info_log);
		d_fail("%s", info_log);
	}

	// fragment shader
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fs, 1, &fs_src, 0);
	glCompileShader(fs);

	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE) {
		glGetShaderInfoLog(fs, 512, NULL, info_log);
		d_fail("%s", info_log);
	}

	// program
	d_app.gl_prog = glCreateProgram();

	glAttachShader(d_app.gl_prog, vs);
	glAttachShader(d_app.gl_prog, fs);

	glBindAttribLocation(d_app.gl_prog, 0, "a_pos");
	glBindAttribLocation(d_app.gl_prog, 1, "a_uv");

	glLinkProgram(d_app.gl_prog);

	glDetachShader(d_app.gl_prog, vs);
	glDetachShader(d_app.gl_prog, fs);
	glDeleteShader(vs);
	glDeleteShader(fs);

	glGetProgramiv(d_app.gl_prog, GL_LINK_STATUS, &success);

	if (success == GL_FALSE) {
		glGetProgramInfoLog(d_app.gl_prog, 512, NULL, info_log);
		d_fail("%s", info_log);
	}

	float verts[] = {
		-1.0,  1.0, 0.0, 0.0,
		 1.0,  1.0, 1.0, 0.0,
		 1.0, -1.0, 1.0, 1.0,
		-1.0, -1.0, 0.0, 1.0,
	};

	unsigned int indices[] = {
		0, 1, 2,
		0, 2, 3,
	};

	// vbuf
	glGenBuffers(1, &d_app.gl_vbuf);
	glBindBuffer(GL_ARRAY_BUFFER, d_app.gl_vbuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// ibuf
	glGenBuffers(1, &d_app.gl_ibuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, d_app.gl_ibuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// tex
	glGenTextures(1, &d_app.gl_tex);
	glBindTexture(GL_TEXTURE_2D, d_app.gl_tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		d_width(),
		d_height(),
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		NULL
	);

	glBindTexture(GL_TEXTURE_2D, 0);

	// init gl
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glViewport(0, 0, sapp_width(), sapp_height());

	d_gfx_init(&d_app.desc);
	d_audio_init(&d_app.desc);
	d_fs_init(&d_app.desc);

	if (d_app.desc.init) {
		d_app.desc.init();
	}

}

static void frame() {

	if (d_app.desc.frame) {
		d_app.desc.frame();
	}

	d_gfx_frame_end();

	// time
	float time = stm_sec(stm_now());

	d_app.dt = time - d_app.time;
	d_app.time = time;

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
	d_app.mouse_dpos = vec2f(0.0, 0.0);
	d_app.char_input = 0;

}

static void event(const sapp_event *e) {

	d_key key = sapp_key_to_d(e->key_code);
	d_mouse mouse = sapp_mouse_to_d(e->mouse_button);

	switch (e->type) {
		case SAPP_EVENTTYPE_KEY_DOWN:
			if (e->key_repeat) {
				d_app.key_states[key] = D_BTN_RPRESSED;
			} else {
				d_app.key_states[key] = D_BTN_PRESSED;
			}
			break;
		case SAPP_EVENTTYPE_KEY_UP:
			d_app.key_states[key] = D_BTN_RELEASED;
			break;
		case SAPP_EVENTTYPE_MOUSE_DOWN:
			d_app.mouse_states[mouse] = D_BTN_PRESSED;
			break;
		case SAPP_EVENTTYPE_MOUSE_UP:
			d_app.mouse_states[mouse] = D_BTN_RELEASED;
			break;
		case SAPP_EVENTTYPE_MOUSE_MOVE:
			d_app.mouse_pos.x = e->mouse_x * d_width() / sapp_width();
			d_app.mouse_pos.y = e->mouse_y * d_height() / sapp_height();
			d_app.mouse_dpos.x = (float)e->mouse_dx;
			d_app.mouse_dpos.y = -(float)e->mouse_dy;
			break;
		case SAPP_EVENTTYPE_MOUSE_SCROLL:
			d_app.wheel.x = -e->scroll_x;
			d_app.wheel.y = -e->scroll_y;
			break;
		case SAPP_EVENTTYPE_CHAR:
			if (isprint(e->char_code)) {
				d_app.char_input = e->char_code;
			}
			break;
		case SAPP_EVENTTYPE_TOUCHES_BEGAN:
			break;
		case SAPP_EVENTTYPE_TOUCHES_MOVED:
			break;
		case SAPP_EVENTTYPE_TOUCHES_ENDED:
		case SAPP_EVENTTYPE_TOUCHES_CANCELLED:
			break;
		case SAPP_EVENTTYPE_RESIZED:
			d_app.resized = true;
			break;
		default:
			break;
	}

}

static void cleanup() {
	if (d_app.desc.quit) {
		d_app.desc.quit();
	}
	d_audio_quit();
}

static void fail(const char *msg) {
	if (d_app.desc.err) {
		d_app.desc.err(msg);
	}
}

void d_run(d_desc desc) {

	desc.title = desc.title ? desc.title : "";
	desc.width = desc.width ? desc.width : 640;
	desc.height = desc.height ? desc.height : 480;
	desc.scale = desc.scale ? desc.scale : 1.0;

	d_app.desc = desc;

	stm_setup();

	sapp_run(&(sapp_desc) {
		.init_cb = init,
		.frame_cb = frame,
		.event_cb = event,
		.cleanup_cb = cleanup,
		.fail_cb = fail,
		.width = desc.width * desc.scale,
		.height = desc.height * desc.scale,
		.high_dpi = desc.hidpi,
		.window_title = desc.title,
		.fullscreen = desc.fullscreen,
	});

}

void d_quit() {
	sapp_quit();
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

void d_present(color *pixels) {

	glClear(GL_COLOR_BUFFER_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, d_app.gl_vbuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, d_app.gl_ibuf);
	glUseProgram(d_app.gl_prog);
	glBindTexture(GL_TEXTURE_2D, d_app.gl_tex);

	glTexSubImage2D(
		GL_TEXTURE_2D,
		0,
		0,
		0,
		d_width(),
		d_height(),
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		pixels
	);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 16, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 16, (void*)8);
	glEnableVertexAttribArray(1);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);
	glBindTexture(GL_TEXTURE_2D, 0);

}

float d_time() {
	return d_app.time;
}

float d_dt() {
	return d_app.dt;
}

void d_set_fullscreen(bool b) {
	if (d_fullscreen() != b) {
		sapp_toggle_fullscreen();
	}
}

bool d_fullscreen() {
	return sapp_is_fullscreen();
}

void d_lock_mouse(bool b) {
	sapp_lock_mouse(b);
}

bool d_mouse_locked() {
	return sapp_mouse_locked();
}

void d_hide_mouse(bool b) {
	sapp_show_mouse(!b);
}

bool d_mouse_hidden() {
	return !sapp_mouse_shown();
}

void d_show_keyboard(bool b) {
	sapp_show_keyboard(b);
}

bool d_keyboard_shown() {
	return false;
}

void d_set_title(const char *title) {
	sapp_set_window_title(title);
}

const char *d_title() {
	return "";
}

bool d_key_pressed(d_key k) {
	return
		d_app.key_states[k] == D_BTN_PRESSED
		;
}

bool d_key_rpressed(d_key k) {
	return
		d_app.key_states[k] == D_BTN_PRESSED
		|| d_app.key_states[k] == D_BTN_RPRESSED
		;
}

bool d_key_down(d_key k) {
	return
		d_app.key_states[k] == D_BTN_PRESSED
		|| d_app.key_states[k] == D_BTN_RPRESSED
		|| d_app.key_states[k] == D_BTN_DOWN
		;
}

bool d_key_released(d_key k) {
	return d_app.key_states[k] == D_BTN_RELEASED;
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
	return d_app.desc.width;
}

int d_height() {
	return d_app.desc.height;
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

