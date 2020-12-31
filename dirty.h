// wengwengweng

/*

ABOUT

  some helper functions

FACTS

  'dirty' is short for 'Dangerous Ichthyologist Reincarnates Tropical Yeti'

DEMO

  #define D_CPU
  #define DIRTY_IMPL
  #include "dirty.h"

  void frame() {
      d_draw_text("hi", vec2f(0, 0));
  }

  int main() {
      d_run((d_desc) {
          .title = "hi",
          .frame = frame,
      });
  }

BUILD

  CFLAGS += -std=c99
  CFLAGS += -O3 (for perf)

  macOS

    CFLAGS += -ObjC
    LDFLAGS += -framework Cocoa
    LDFLAGS += -framework AudioToolbox
    LDFLAGS += -framework Metal (D_METAL)
    LDFLAGS += -framework MetalKit (D_METAL)
    LDFLAGS += -framework OpenGL (D_GL)

  iOS

    CC := xcrun -sdk iphoneos clang (for device)
    CC := xcrun -sdk iphonesimulator clang (for sim)
    CFLAGS += -ObjC
    CFLAGS += -arch armv7 (for device)
    CFLAGS += -arch arm64 (for device)
    CFLAGS += -arch x86_64 (for sim)
    LDFLAGS += -framework Foundation
    LDFLAGS += -framework UIKit
    LDFLAGS += -framework AudioToolbox
    LDFLAGS += -framework Metal (D_METAL)
    LDFLAGS += -framework MetalKit (D_METAL)
    LDFLAGS += -framework OpenGLES (D_GL)
    LDFLAGS += -framework GLKit (D_GL)

  Linux

    LDFLAGS += -lX11
    LDFLAGS += -lGL (D_GL)

  Web

    CC := emcc

*/

#ifndef DIRTY_H
#define DIRTY_H

#include <stdbool.h>
#include <stdint.h>

#if defined(__APPLE__)
	#include <TargetConditionals.h>
	#if TARGET_OS_OSX
		#define D_MACOS
	#elif TARGET_OS_IOS
		#define D_IOS
	#endif
#elif defined(__EMSCRIPTEN__)
	#define D_WEB
#elif defined(_WIN32) || defined(_WIN64)
	#define D_WINDOWS
#elif defined(__ANDROID__)
	#define D_ANDROID
#elif defined(__linux__) || defined(__unix__)
	#define D_LINUX
#endif

#if \
	!defined(D_MACOS) && \
	!defined(D_IOS) && \
	!defined(D_WEB) && \
	!defined(D_WINDOWS) && \
	!defined(D_ANDROID) && \
	!defined(D_LINUX)
	#error "platform not supported"
#endif

// math     *                       .            ~     +    .
//     .           .            ~          +
//             +          .                          .
//               .                      .
//  @      .        ~           .                 @            +
//                                        +
//      .                                                 ~
//          ~            +           +
//               +                .      .               +
//       ~                   .                 +               ~
//    .       @        .                   ~           .
//                                .                           .

#define D_PI 3.14
#define D_RNG_A 1103515245
#define D_RNG_C 12345
#define D_RNG_M 2147483648

typedef struct {
	uint64_t seed;
} d_rng;

typedef struct {
	float x;
	float y;
} vec2;

typedef struct {
	float x;
	float y;
	float z;
} vec3;

typedef struct {
	float x;
	float y;
	float z;
	float w;
} vec4;

typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
} color;

typedef struct {
	float m[16];
} mat4;

typedef struct {
	float x;
	float y;
	float z;
	float w;
} quat;

typedef struct {
	float x;
	float y;
	float w;
	float h;
} quad;

typedef struct {
	vec2 p1;
	vec2 p2;
} rect;

typedef struct {
	vec3 p1;
	vec3 p2;
} box;

typedef struct {
	vec2 p1;
	vec2 p2;
} line2;

typedef struct {
	vec3 p1;
	vec3 p2;
} line3;

typedef struct {
	vec2 center;
	float radius;
} circle;

typedef struct {
	vec3 center;
	float radius;
} sphere;

typedef struct {
	vec3 normal;
	float dist;
} plane;

typedef struct {
	vec3 origin;
	vec3 dir;
} ray3;

typedef struct {
	vec2 origin;
	vec2 dir;
} ray2;

d_rng d_make_rng(uint64_t seed);
float d_rng_gen(d_rng *rng);
float randf(float, float);

vec2 vec2f(float, float);
vec2 vec2u();
vec2 vec2_add(vec2, vec2);
vec2 vec2_sub(vec2, vec2);
vec2 vec2_scale(vec2, float);
vec2 vec2_mult(vec2, vec2);
float vec2_dist(vec2, vec2);
float vec2_len(vec2);
vec2 vec2_unit(vec2);
vec3 vec2_cross(vec2, vec2);
float vec2_dot(vec2, vec2);
vec2 vec2_normal(vec2);
float vec2_angle(vec2, vec2);
vec2 vec2_min(vec2, vec2);
vec2 vec2_max(vec2, vec2);
vec2 vec2_rand(vec2, vec2);
vec2 vec2_lerp(vec2, vec2, float);
vec2 vec2_clamp(vec2, vec2, vec2);
bool vec2_eq(vec2, vec2);
const char *vec2_fmt(vec2);

vec3 vec3f(float, float, float);
vec3 vec3u();
vec3 vec3_add(vec3, vec3);
vec3 vec3_sub(vec3, vec3);
vec3 vec3_scale(vec3, float);
vec3 vec3_mult(vec3, vec3);
float vec3_dist(vec3, vec3);
float vec3_len(vec3);
vec3 vec3_unit(vec3);
vec3 vec3_cross(vec3, vec3);
float vec3_dot(vec3, vec3);
vec3 vec3_min(vec3, vec3);
vec3 vec3_max(vec3, vec3);
vec3 vec3_rand(vec3, vec3);
vec3 vec3_lerp(vec3, vec3, float);
vec3 vec3_clamp(vec3, vec3, vec3);
bool vec3_eq(vec3, vec3);
const char *vec3_fmt(vec3);

vec4 vec4f(float, float, float, float);
vec4 vec4u();

color colori(uint8_t, uint8_t, uint8_t, uint8_t);
color colorf(float, float, float, float);
color colorx(uint32_t hex);
color color_mix(color c1, color c2);
color coloru();
bool color_eq(color, color);
const char *color_fmt(color);

mat4 mat4f(
	float, float, float, float,
	float, float, float, float,
	float, float, float, float,
	float, float, float, float
);
mat4 mat4u();
mat4 mat4_identity();
mat4 mat4_mult(mat4, mat4);
vec4 mat4_mult_vec4(mat4, vec4);
vec3 mat4_mult_vec3(mat4, vec3);
vec2 mat4_mult_vec2(mat4, vec2);
mat4 mat4_invert(mat4);
mat4 mat4_scale(vec3);
mat4 mat4_translate(vec3);
mat4 mat4_ortho(float w, float n, float near, float far);
mat4 mat4_persp(float fov, float aspect, float far, float near);
mat4 mat4_view(vec3 pos, vec3 dir, vec3 up);
mat4 mat4_rot_x(float);
mat4 mat4_rot_y(float);
mat4 mat4_rot_z(float);
mat4 mat4_rot_quat(quat);
const char *mat4_fmt(mat4);

quat quatf(float, float, float, float);
quat quatu();

quad quadf(float, float, float, float);
quad quadu();
const char *quad_fmt(quad);

rect rectf(vec2, vec2);
box boxf(vec3, vec3);
line2 line2f(vec2, vec2);
line3 line3f(vec3, vec3);
circle circlef(vec2, float);
sphere spheref(vec3, float);
plane planef(vec3, float);
ray2 ray2f(vec2, vec2);
ray3 ray3f(vec3, vec3);

float degf(float);
float radf(float);
float clampf(float, float, float);
int clampi(int, int, int);
float lerpf(float, float, float);
float mapf(float, float, float, float, float);

bool pt_rect(vec2 pt, rect r);
bool rect_rect(rect r1, rect r2);
bool line_line(line2 l1, line2 l2);
bool line_rect(line2 l, rect r);
bool pt_circle(vec2 pt, circle c);
bool circle_circle(circle c1, circle c2);
bool line_circle(line2 l, circle c);

// app      *                       .            ~     +    .
//     .           .            ~          +
//             +          .                          .
//               .                      .
//  @      .        ~           .                 @            +
//                                        +
//      .                                                 ~
//          ~            +           +
//               +                .      .               +
//       ~                   .                 +               ~
//    .       @        .                   ~           .
//                                .                           .

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
	_D_NUM_KEYS,
} d_key;

typedef enum {
	D_KMOD_ALT,
	D_KMOD_META,
	D_KMOD_CTRL,
	D_KMOD_SHIFT,
} d_kmod;

typedef enum {
	D_MOUSE_NONE,
	D_MOUSE_LEFT,
	D_MOUSE_RIGHT,
	D_MOUSE_MIDDLE,
	_D_NUM_MOUSE,
} d_mouse;

typedef enum {
	D_STRETCH,
	D_FIT_BIG,
	D_FIT_SMALL,
} d_scale_mode;

typedef struct {
	void (*init)();
	void (*frame)();
	float (*stream)();
	void (*quit)();
	const char *title;
	int width;
	int height;
	float scale;
	d_scale_mode scale_mode;
	bool fullscreen;
	bool vsync;
	bool hidpi;
	const char *path;
	const char *org;
	const char *name;
	color clear_color;
	const char *canvas_root;
	void *udata;
} d_desc;

typedef uint8_t d_touch;

void d_run(d_desc desc);
void d_quit();
void d_fail(const char *fmt, ...);
void d_assert(bool test, const char *fmt, ...);

bool d_fullscreen();
void d_set_fullscreen(bool b);

bool d_mouse_locked();
void d_lock_mouse(bool b);

bool d_mouse_hidden();
void d_hide_mouse(bool b);

void d_set_title(const char *title);

bool d_keyboard_shown();
void d_show_keyboard(bool b);

int d_width();
int d_height();
int d_win_width();
int d_win_height();

float d_time();
float d_dt();
int d_fps();

bool d_key_pressed(d_key k);
bool d_key_rpressed(d_key k);
bool d_key_released(d_key k);
bool d_mouse_pressed(d_mouse m);
bool d_mouse_released(d_mouse m);
bool d_mouse_moved();
bool d_touch_pressed(d_touch t);
bool d_touch_released(d_touch t);
bool d_touch_moved(d_touch t);
bool d_scrolled();
vec2 d_wheel();
bool d_resized();
char d_input();
bool d_active();

bool d_mouse_down(d_mouse m);
bool d_key_down(d_key k);
bool d_key_mod(d_kmod kmod);
vec2 d_mouse_pos();
vec2 d_mouse_dpos();
vec2 d_touch_pos(d_touch t);
vec2 d_touch_dpos(d_touch t);

void *d_udata();

// gfx      *                       .            ~     +    .
//     .           .            ~          +
//             +          .                          .
//               .                      .
//  @      .        ~           .                 @            +
//                                        +
//      .                                                 ~
//          ~            +           +
//               +                .      .               +
//       ~                   .                 +               ~
//    .       @        .                   ~           .
//                                .                           .

typedef enum {
	D_CENTER,
	D_TOP_LEFT,
	D_TOP,
	D_TOP_RIGHT,
	D_LEFT,
	D_RIGHT,
	D_BOT_LEFT,
	D_BOT,
	D_BOT_RIGHT,
} d_origin;

typedef enum {
	D_ALPHA,
	D_REPLACE,
	D_ADD,
} d_blend;

typedef enum {
	D_BORDER,
	D_EDGE,
	D_REPEAT,
} d_wrap;

typedef struct {
	vec3 pos;
	vec3 normal;
	vec2 uv;
	color color;
} d_vertex;

typedef unsigned int d_index;

typedef struct {
	vec3 pos;
	vec3 scale;
	quat rot;
} d_psr;

typedef struct {
	int width;
	int height;
	color *pixels;
} d_img;

typedef struct {
	char name[16];
	int from;
	int to;
	bool loop;
} d_anim;

typedef struct {
	int num_frames;
	d_img *frames;
	int num_anims;
	d_anim *anims;
} d_sprite;

typedef struct {
	int gw;
	int gh;
	int rows;
	int cols;
	int map[128];
	uint8_t *pixels;
} d_font;

typedef struct {
	d_vertex *verts;
	int num_verts;
	d_index *indices;
	int num_indices;
} d_mesh;

typedef struct d_model_node {
	d_psr psr;
	struct d_model_node *children;
	int num_children;
	d_mesh *meshes;
	int num_meshes;
} d_model_node;

typedef struct {
	d_model_node *nodes;
	int num_nodes;
	d_img *images;
	int num_images;
} d_model;

d_img d_make_img(int w, int h);
d_img d_parse_img(const uint8_t *bytes);
d_img d_load_img(const char *path);
void d_img_set(d_img *img, int x, int y, color c);
void d_img_set_ex(d_img *img, int x, int y, color c, d_blend blend);
color d_img_get(const d_img *img, int x, int y);
color d_img_get_ex(const d_img *img, int x, int y, d_wrap wrap);
void d_img_fill(d_img *img, color c);
void d_img_save(const d_img *img, const char *path);
d_img d_img_clone(const d_img *img);
void d_free_img(d_img *img);

d_font d_parse_font(const uint8_t *bytes);
void d_free_font(d_font *font);

void d_free_mesh(d_mesh *mesh);

void d_clear();
void d_set_blend(d_blend b);
void d_set_wrap(d_wrap w);
void d_put(int x, int y, color c);
color d_peek(int x, int y);
void d_draw_img(const d_img *img, vec2 pos);
void d_draw_text(const char *text, vec2 pos);
void d_draw_tri(vec2 p1, vec2 p2, vec2 p3, color c);
void d_draw_rect(vec2 p1, vec2 p2, color c);
void d_draw_circle(vec2 center, float r, color c);
void d_draw_line(vec2 p1, vec2 p2, color c);
void d_draw_mesh(const d_mesh *mesh);
void d_draw_model(const d_model *model);

void d_drawon(d_img *img);
d_img *d_canvas();

// audio    *                       .            ~     +    .
//     .           .            ~          +
//             +          .                          .
//               .                      .
//  @      .        ~           .                 @            +
//                                        +
//      .                                                 ~
//          ~            +           +
//               +                .      .               +
//       ~                   .                 +               ~
//    .       @        .                   ~           .
//                                .                           .

// a static sound
typedef struct {
	short *frames;
	int num_frames;
} d_sound;

// sound play config
typedef struct {
	bool loop;
	float volume;
	bool paused;
	float time;
} d_play_conf;

// sound playback control handle
typedef struct {
	const d_sound *src;
	int pos;
	bool loop;
	float volume;
	bool paused;
	bool done;
} d_playback;

typedef struct {
	float attack;
	float decay;
	float sustain;
	float release;
} d_envelope;

typedef struct {
	float life;
	float afterlife;
	float volume;
	bool active;
	bool alive;
} d_voice;

// user provided stream
void d_stream(float (*f)());

// SOUND
d_sound d_make_sound(const short *frames, int num_frames);
d_sound d_parse_sound(const uint8_t *bytes);
d_sound d_load_sound(const char *path);
float d_sound_sample(const d_sound *snd, float time);
float d_sound_len(const d_sound *snd);
void d_free_sound(d_sound *sound);
// play a sound, returning a handle for control
d_playback *d_play(const d_sound *sound);
d_playback *d_play_ex(const d_sound *sound, d_play_conf conf);
void d_playback_seek(d_playback *pb, float time);
float d_playback_time(d_playback *pb);

// SYNTH
void d_synth_play(int note);
void d_synth_release(int note);
d_envelope *d_synth_envelope();
void d_synth_wav(float (*func)(float freq, float t));
float d_synth_peek(int n);

// voice
d_voice d_make_voice();
void d_voice_process(d_voice *v, const d_envelope *e, float dt);

// built in wave forms
float d_wav_sin(float freq, float t);
float d_wav_square(float freq, float t);
float d_wav_tri(float freq, float t);
float d_wav_saw(float freq, float t);
float d_wav_noise(float freq, float t);

// fs       *                       .            ~     +    .
//     .           .            ~          +
//             +          .                          .
//               .                      .
//  @      .        ~           .                 @            +
//                                        +
//      .                                                 ~
//          ~            +           +
//               +                .      .               +
//       ~                   .                 +               ~
//    .       @        .                   ~           .
//                                .                           .

#include <stdlib.h>

// read resource file content (alloc)
char *d_read_text(const char *path);
uint8_t *d_read_bytes(const char *path, size_t *size);
char **d_glob(const char *path, const char *ext);
void d_free_dir(char **list);
bool d_is_file(const char *path);
bool d_is_dir(const char *path);

// read / write data file content (alloc)
char *d_data_read_text(const char *path);
uint8_t *d_data_read_bytes(const char *path, size_t *size);
void d_data_write_text(const char *path, const char *content);
void d_data_write_bytes(const char *path, const uint8_t *content, size_t size);
bool d_data_is_file(const char *path);
bool d_data_is_dir(const char *path);

char *d_extname(const char *path);
char *d_basename(const char *path);

// utils     *                       .            ~     +    .
//     .           .            ~          +
//             +          .                          .
//               .                      .
//  @      .        ~           .                 @            +
//                                        +
//      .                                                 ~
//          ~            +           +
//               +                .      .               +
//       ~                   .                 +               ~
//    .       @        .                   ~           .
//                                .                           .

// format text (one-off)
const char *d_fmt(const char *fmt, ...);
// format text (alloc)
char *d_fmta(const char *fmt, ...);

#endif // DIRTY_H


// header
//         *                       .            ~     +    .
//     .           .            ~          +
//             +          .                          .              .
//               .                      .
//  @      .        ~           .                 @            +
//              !                         +
//      .      -o-                                         ~
//              !        +           +                            .
//                               .      .               +
//   .                                          .
//                 .                                                 .
//                         ~                                    +
//                     .            .
//         @                              /           .
//                  .                    /                   +
//   ~                         +        *
//              .
//                                           +                     .
//                    .
//                                                 .
//          .                                                 .
//                      ,--
//                     / .  \        .
//                     \    /                ^v^
//   .                   --.
//                                                                 .
//                                         .
//              ^v^                              --------
//                             .                / DIRTY /
//           _ _ ,- .                           --------           __,,-,
//       _,-' '` .'   ` .  .._                 ./,-- /....,--''''` .
//    ,-' .  '  .  '  .  '  . ` ..          --' .  '  .  '  .  '  .  '
// .-''  '` .  '` .  '` .  '` .  '`--..__ ,` .  '` .  '` .  '` .  '` .
//   .    .  '  .  '  .  '  .  '  .  '  '\.  '  .  '  .  '  .  '  .  '
// ` .  '` .  '  .  '` .  '` .   ` .  '` .`.   ` .  '` .  '` .   ` .  '
//  .  `      .    '  .     .   `    .       .      `       .  '
// .     '  .      .       `       .       .`      .       '`   .    `
//   .         .     `   .   '   .      '`        .    .`      '`   .
//     `   .      '`           .       '      .     `         .   `    '
//  '.  `    .  '`    '`  .   `     .`     .   ` .   `     ' .   '`
// .     '  .      .       `       .       .`      .       '`   .    `
//  ~ ` ~                                 - ~~
//   .     ^ -- ~    `   .   '   .    ~~`        ~    .`      '`   .
//         .      '` ~  -~ -  ~~  ^   o   '      . ^   ~ ~    ~     `
//     .      o                          .                  .     ~
//            .         ~    _     .                    o             ~
//      o        ~         ><_'>             ~            .
// impl            .        .                     O             ~

#ifdef DIRTY_IMPL
#ifndef DIRTY_IMPL_ONCE
#define DIRTY_IMPL_ONCE

// res                    ~                       o                   ~
//         ~         .                 ~          .          O
//   .         _              O                              o
//        .  ><_'>            o           .         .        .
//                            .               .                    .
//  .                   ~          .                  ~
//              o           ~               ~                   .
//      O       .               .        _                  O
//      o                .             <'_><        .       o         .
//      .    .                                              .
//                ~         .     o             ~                   .
//   ~               .            .          .                 ~

static uint8_t unscii_bytes[] = {
	0x08, 0x08, 0x13, 0x05, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
	0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33,
	0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b,
	0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
	0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63,
	0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b,
	0x7c, 0x7d, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00,
	0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01,
	0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00,
	0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x01, 0x01, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01,
	0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x01, 0x01, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// app                    ~                       o                   ~
//         ~         .                 ~          .          O
//   .         _              O                              o
//        .  ><_'>            o           .         .        .
//                            .               .                    .
//  .                   ~          .                  ~
//              o           ~               ~                   .
//      O       .               .        _                  O
//      o                .             <'_><        .       o         .
//      .    .                                              .
//                ~         .     o             ~                   .
//   ~               .            .          .                 ~

// TODO: scale strategies
// TODO: try CVDisplayLink?

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <sys/time.h>

#if !defined(D_CPU) && !defined(D_GL) && !defined(D_METAL) && !defined(D_TERM)
	#error "must define a blit method (D_CPU, D_GL, D_METAL, D_TERM)"
#endif

#if defined(D_METAL) && !defined(D_MACOS) && !defined(D_IOS)
	#error "D_METAL is only for macOS or iOS"
#endif

#if \
	!defined(D_COCOA) && \
	!defined(D_UIKIT) && \
	!defined(D_X11) && \
	!defined(D_CANVAS) && \
	!defined(D_TERM)
	#if defined(D_MACOS)
		#define D_COCOA
	#elif defined(D_IOS)
		#define D_UIKIT
	#elif defined(D_LINUX)
		#define D_X11
	#elif defined(D_WEB)
		#define D_CANVAS
	#endif
#endif

#if defined(D_GL)

	#define GL_SILENCE_DEPRECATION
	#define GLES_SILENCE_DEPRECATION

	#if defined(D_MACOS)
		#include <OpenGL/gl.h>
	#elif defined(D_IOS)
		#define D_GLES
		#include <OpenGLES/ES2/gl.h>
	#elif defined(D_LINUX)
		#include <GL/gl.h>
	#elif defined(D_ANDROID)
		#define D_GLES
		#include <GLES2/gl2.h>
	#elif defined(D_WINDOWS)
		#include <GL/gl.h>
	#elif defined(D_WEB)
		#define D_GLES
		#include <GLES2/gl2.h>
	#endif

#endif // D_GL

#if defined(D_METAL)
	#import <Metal/Metal.h>
	#import <MetalKit/MetalKit.h>
#endif

#if defined(D_COCOA)
	#import <Cocoa/Cocoa.h>
#elif defined(D_UIKIT)
	#import <UIKit/UIKit.h>
#elif defined(D_CANVAS)
	#include <emscripten/emscripten.h>
#elif defined(D_X11)
	#include <X11/Xlib.h>
	#if defined(D_GL)
		#include <GL/glx.h>
	#endif
#elif defined(D_WAYLAND)
	#include <wayland-client.h>
#endif

#define D_MAX_TOUCHES 8
#define TXT(src) #src

static float quad_verts[] = {
	-1, -1, 0, 1,
	-1, 1, 0, 0,
	1, -1, 1, 1,
	1, 1, 1, 0,
};

void d_gfx_init(const d_desc *desc);
void d_audio_init(const d_desc *desc);
void d_fs_init(const d_desc *desc);
void d_gfx_frame_end();

#if defined(D_COCOA)

@interface DAppDelegate : NSObject<NSApplicationDelegate>
	-(void)loop:(NSTimer*) timer;
@end

@interface DWindowDelegate : NSObject<NSWindowDelegate>
@end

#if defined(D_GL)
@interface DView : NSOpenGLView
#elif defined(D_METAL)
@interface DView : MTKView
#else
@interface DView : NSView
#endif
@end

#elif defined(D_UIKIT)

#if defined(D_CPU)
@interface DView : UIView
#elif defined(D_GL)
@interface DView : UIView
#elif defined(D_METAL)
@interface DView : MTKView
#endif
@end

@interface DAppDelegate : NSObject<UIApplicationDelegate>
	-(void)loop:(NSTimer*) timer;
@end

#endif

typedef enum {
	D_BTN_IDLE,
	D_BTN_PRESSED,
	D_BTN_RPRESSED,
	D_BTN_RELEASED,
	D_BTN_DOWN,
} d_btn;

typedef struct {
	uintptr_t id;
	vec2 pos;
	vec2 dpos;
	d_btn state;
} d_touch_state;

typedef struct {

	d_desc desc;
	const color *buf;
	struct timeval start_time;
	float time;
	float dt;
	int width;
	int height;
	int win_width;
	int win_height;
	vec2 mouse_pos;
	vec2 mouse_dpos;
	vec2 wheel;
	d_btn key_states[_D_NUM_KEYS];
	d_btn mouse_states[_D_NUM_MOUSE];
	d_touch_state touches[D_MAX_TOUCHES];
	int num_touches;
	bool resized;
	char char_input;
	float fps_timer;
	int fps;
	bool quit;

#if defined(D_COCOA)
	NSWindow *window;
	DView *view;
#elif defined(D_UIKIT)
	UIWindow *window;
	DView *view;
#elif defined(D_X11)
	Display *display;
	Window window;
#endif

#if defined(D_METAL)
	id<MTLDevice> mtl_dev;
	id<MTLCommandQueue> mtl_queue;
	id<MTLTexture> mtl_tex;
	id<MTLRenderPipelineState> mtl_pip;
#endif

#if defined(D_GL)
	GLuint gl_prog;
	GLuint gl_vbuf;
	GLuint gl_tex;
#endif

} d_app_ctx;

static d_app_ctx d_app;

static void d_present(const color *canvas) {
	d_app.buf = canvas;
}

static void d_process_btn(d_btn *b) {
	if (*b == D_BTN_PRESSED || *b == D_BTN_RPRESSED) {
		*b = D_BTN_DOWN;
	} else if (*b == D_BTN_RELEASED) {
		*b = D_BTN_IDLE;
	}
}

static void d_app_init() {

	// TODO: can't call [window toggleFullScreen] before d_gfx_init()
	// on D_GL and D_CPU, why?
	d_gfx_init(&d_app.desc);
	d_audio_init(&d_app.desc);
	d_fs_init(&d_app.desc);
	gettimeofday(&d_app.start_time, NULL);

	if (d_app.desc.init) {
		d_app.desc.init();
	}

}

static void d_app_frame() {

	if (d_app.desc.frame) {
		d_app.desc.frame();
	}

	d_gfx_frame_end();

	// time
	struct timeval time;
	gettimeofday(&time, NULL);
	float t =
		(float)(time.tv_sec - d_app.start_time.tv_sec)
		+ (float)(time.tv_usec - d_app.start_time.tv_usec) / 1000000.0;
	d_app.dt = t - d_app.time;
	d_app.time = t;
	d_app.fps_timer += d_app.dt;

	if (d_app.fps_timer >= 1.0) {
		d_app.fps_timer = 0.0;
		d_app.fps = (int)(1.0 / d_app.dt);
	}

	// reset input states
	for (int i = 0; i < _D_NUM_KEYS; i++) {
		d_process_btn(&d_app.key_states[i]);
	}

	for (int i = 0; i < _D_NUM_MOUSE; i++) {
		d_process_btn(&d_app.mouse_states[i]);
	}

	for (int i = 0; i < d_app.num_touches; i++) {
		d_process_btn(&d_app.touches[i].state);
		if (d_app.touches[i].state == D_BTN_IDLE) {
			d_app.touches[i] = d_app.touches[d_app.num_touches - 1];
			d_app.num_touches--;
			i--;
		}
	}

	d_app.wheel.x = 0.0;
	d_app.wheel.y = 0.0;
	d_app.resized = false;
	d_app.mouse_dpos = vec2f(0.0, 0.0);
	d_app.char_input = 0;

}

// -------------------------------------------------------------
// OpenGL
#if defined(D_GL)

static const char *vs_src =
#ifndef D_GLES
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
#ifndef D_GLES
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

static void d_gl_init() {

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

	// vbuf
	glGenBuffers(1, &d_app.gl_vbuf);
	glBindBuffer(GL_ARRAY_BUFFER, d_app.gl_vbuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_verts), quad_verts, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// tex
	glGenTextures(1, &d_app.gl_tex);
	glBindTexture(GL_TEXTURE_2D, d_app.gl_tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

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
	glViewport(0, 0, d_win_width(), d_win_height());
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

}

static void d_gl_blit() {

	glViewport(0, 0, d_win_width(), d_win_height());
	glClear(GL_COLOR_BUFFER_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, d_app.gl_vbuf);
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
		d_app.buf
	);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 16, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 16, (void*)8);
	glEnableVertexAttribArray(1);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFlush();

}

#endif // D_GL

// -------------------------------------------------------------
// Metal
#if defined(D_METAL)

static const char *shader_src =
"using namespace metal;"
"struct VertexIn {"
	"float2 pos;"
	"float2 uv;"
"};"
"struct VertexOut {"
	"float4 pos [[position]];"
	"float2 uv;"
"};"
"vertex VertexOut vert("
	"const device VertexIn *verts [[ buffer(0) ]],"
	"unsigned int id [[ vertex_id ]]"
") {"
	"VertexIn in = verts[id];"
	"VertexOut out;"
	"out.pos = float4(in.pos, 0.0, 1.0);"
	"out.uv = in.uv;"
	"return out;"
"}"
"fragment float4 frag("
	"VertexOut in [[ stage_in ]],"
	"texture2d<half> tex [[ texture(0) ]]"
") {"
	"constexpr sampler samp(mag_filter::nearest, min_filter::nearest);"
	"return float4(tex.sample(samp, in.uv));"
"}"
;

static void d_mtl_init() {

	id<MTLDevice> dev = MTLCreateSystemDefaultDevice();
	id<MTLCommandQueue> queue = [dev newCommandQueue];

	NSError *err = NULL;

	id<MTLLibrary> lib = [dev
		newLibraryWithSource:[NSString stringWithUTF8String:shader_src]
		options:nil
		error:&err
	];

	if (err) {
		d_fail("%s\n", [err.localizedDescription UTF8String]);
	}

	MTLRenderPipelineDescriptor *pip_desc = [[MTLRenderPipelineDescriptor alloc] init];
	pip_desc.vertexFunction = [lib newFunctionWithName:@"vert"];
	pip_desc.fragmentFunction = [lib newFunctionWithName:@"frag"];
	pip_desc.colorAttachments[0].pixelFormat = MTLPixelFormatRGBA8Unorm;

	id<MTLRenderPipelineState> pip = [dev
		newRenderPipelineStateWithDescriptor:pip_desc
		error:&err
	];

	if (err) {
		d_fail("%s\n", [err.localizedDescription UTF8String]);
	}

	MTLTextureDescriptor *tex_desc = [MTLTextureDescriptor
		texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
		width:d_app.width
		height:d_app.height
		mipmapped:false
	];

	id<MTLTexture> tex = [dev newTextureWithDescriptor:tex_desc];

	d_app.mtl_dev = dev;
	d_app.mtl_queue = queue;
	d_app.mtl_pip = pip;
	d_app.mtl_tex = tex;
	d_app.view.device = dev;

}

static void d_mtl_blit() {

	id<MTLCommandBuffer> cmd_buf = [d_app.mtl_queue commandBuffer];
	MTLRenderPassDescriptor *desc = [d_app.view currentRenderPassDescriptor];

	MTLRegion region = {
		{ 0, 0, 0 },
		{ d_app.width, d_app.height, 1 }
	};

	[d_app.mtl_tex
		replaceRegion:region
		mipmapLevel:0
		withBytes:d_app.buf
		bytesPerRow:d_app.width * 4
	];

	id<MTLRenderCommandEncoder> encoder = [cmd_buf
		renderCommandEncoderWithDescriptor:desc
	];

	[encoder
		setVertexBytes:quad_verts
		length:sizeof(quad_verts)
		atIndex:0
	];

	[encoder
		setRenderPipelineState:d_app.mtl_pip
	];

	[encoder
		setFragmentTexture:d_app.mtl_tex
		atIndex:0
	];

	[encoder
		drawPrimitives:MTLPrimitiveTypeTriangleStrip
		vertexStart:0
		vertexCount:4
	];

	[encoder endEncoding];

	[cmd_buf
		presentDrawable:[d_app.view currentDrawable]
	];

	[cmd_buf commit];

}

#endif

// -------------------------------------------------------------
// Cocoa
#if defined(D_COCOA)

static d_key d_cocoa_key(unsigned short k) {
	switch (k) {
		case 0x1D: return D_KEY_0;
		case 0x12: return D_KEY_1;
		case 0x13: return D_KEY_2;
		case 0x14: return D_KEY_3;
		case 0x15: return D_KEY_4;
		case 0x17: return D_KEY_5;
		case 0x16: return D_KEY_6;
		case 0x1A: return D_KEY_7;
		case 0x1C: return D_KEY_8;
		case 0x19: return D_KEY_9;
		case 0x00: return D_KEY_A;
		case 0x0B: return D_KEY_B;
		case 0x08: return D_KEY_C;
		case 0x02: return D_KEY_D;
		case 0x0E: return D_KEY_E;
		case 0x03: return D_KEY_F;
		case 0x05: return D_KEY_G;
		case 0x04: return D_KEY_H;
		case 0x22: return D_KEY_I;
		case 0x26: return D_KEY_J;
		case 0x28: return D_KEY_K;
		case 0x25: return D_KEY_L;
		case 0x2E: return D_KEY_M;
		case 0x2D: return D_KEY_N;
		case 0x1F: return D_KEY_O;
		case 0x23: return D_KEY_P;
		case 0x0C: return D_KEY_Q;
		case 0x0F: return D_KEY_R;
		case 0x01: return D_KEY_S;
		case 0x11: return D_KEY_T;
		case 0x20: return D_KEY_U;
		case 0x09: return D_KEY_V;
		case 0x0D: return D_KEY_W;
		case 0x07: return D_KEY_X;
		case 0x10: return D_KEY_Y;
		case 0x06: return D_KEY_Z;
		case 0x27: return D_KEY_QUOTE;
		case 0x2A: return D_KEY_BACKSLASH;
		case 0x2B: return D_KEY_COMMA;
		case 0x18: return D_KEY_EQUAL;
		case 0x32: return D_KEY_BACKQUOTE;
		case 0x21: return D_KEY_LBRACKET;
		case 0x1B: return D_KEY_MINUS;
		case 0x2F: return D_KEY_PERIOD;
		case 0x1E: return D_KEY_RBRACKET;
		case 0x29: return D_KEY_SEMICOLON;
		case 0x2C: return D_KEY_SLASH;
		case 0x33: return D_KEY_BACKSPACE;
		case 0x7D: return D_KEY_DOWN;
		case 0x24: return D_KEY_ENTER;
		case 0x35: return D_KEY_ESC;
		case 0x7A: return D_KEY_F1;
		case 0x78: return D_KEY_F2;
		case 0x63: return D_KEY_F3;
		case 0x76: return D_KEY_F4;
		case 0x60: return D_KEY_F5;
		case 0x61: return D_KEY_F6;
		case 0x62: return D_KEY_F7;
		case 0x64: return D_KEY_F8;
		case 0x65: return D_KEY_F9;
		case 0x6D: return D_KEY_F10;
		case 0x67: return D_KEY_F11;
		case 0x6F: return D_KEY_F12;
		case 0x7B: return D_KEY_LEFT;
		case 0x3A: return D_KEY_LALT;
		case 0x3B: return D_KEY_LCTRL;
		case 0x38: return D_KEY_LSHIFT;
		case 0x37: return D_KEY_LMETA;
		case 0x7C: return D_KEY_RIGHT;
		case 0x3D: return D_KEY_RALT;
		case 0x3E: return D_KEY_RCTRL;
		case 0x3C: return D_KEY_RSHIFT;
		case 0x36: return D_KEY_RMETA;
		case 0x31: return D_KEY_SPACE;
		case 0x30: return D_KEY_TAB;
		case 0x7E: return D_KEY_UP;
	}
	return D_KEY_NONE;
}

@implementation DAppDelegate
- (void)applicationDidFinishLaunching:(NSNotification*)noti {

	NSWindow *window = [[NSWindow alloc]
		initWithContentRect:NSMakeRect(0, 0, d_app.win_width, d_app.win_height)
		styleMask:
			0
			| NSWindowStyleMaskTitled
			| NSWindowStyleMaskClosable
			| NSWindowStyleMaskResizable
			| NSWindowStyleMaskMiniaturizable
		backing:NSBackingStoreBuffered
		defer:NO
	];

	d_app.window = window;

	if (d_app.desc.title) {
		[window setTitle:[NSString stringWithUTF8String:d_app.desc.title]];
	}

	[window setAcceptsMouseMovedEvents:YES];
	[window center];
	[window setDelegate:[[DWindowDelegate alloc] init]];
	DView *view = [[DView alloc] init];
	d_app.view = view;
	[window setContentView:view];
	[window makeFirstResponder:view];
	[window makeKeyAndOrderFront:nil];

#if defined(D_GL)

	NSOpenGLContext *ctx = [view openGLContext];

	if (d_app.desc.hidpi) {
		[view setWantsBestResolutionOpenGLSurface:YES];
	}

	[ctx
		setValues:(int*)&d_app.desc.vsync
		forParameter:NSOpenGLContextParameterSwapInterval
	];

	[ctx makeCurrentContext];

	d_gl_init();

#elif defined(D_METAL)
	d_mtl_init();
#endif

	d_app_init();

	if (d_app.desc.fullscreen) {
		d_set_fullscreen(true);
	}

	[NSTimer
		scheduledTimerWithTimeInterval:0.001
		target:self
		selector:@selector(loop:)
		userInfo:nil
		repeats:YES
	];

}
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender {
	return YES;
}
-(void)loop:(NSTimer*)timer {
	[d_app.view setNeedsDisplay:YES];
}
@end

@implementation DWindowDelegate
- (void)windowDidResize:(NSNotification*)noti {
	NSSize size = [d_app.view frame].size;
	d_app.win_width = size.width;
	d_app.win_height = size.height;
	d_app.resized = true;
}
@end

@implementation DView
// - (BOOL)isOpaque {
// 	return YES;
// }
- (BOOL)canBecomeKeyView {
	return YES;
}
- (BOOL)acceptsFirstResponder {
	return YES;
}
- (void)keyDown:(NSEvent*)event {
	d_key k = d_cocoa_key(event.keyCode);
	if (k) {
		if (event.ARepeat) {
			d_app.key_states[k] = D_BTN_RPRESSED;
		} else {
			d_app.key_states[k] = D_BTN_PRESSED;
		}
	}
}
- (void)keyUp:(NSEvent*)event {
	d_key k = d_cocoa_key(event.keyCode);
	if (k) {
		d_app.key_states[k] = D_BTN_RELEASED;
	}
}
- (void)mouseDown:(NSEvent*)event {
	d_app.mouse_states[D_MOUSE_LEFT] = D_BTN_PRESSED;
}
- (void)mouseUp:(NSEvent*)event {
	d_app.mouse_states[D_MOUSE_LEFT] = D_BTN_RELEASED;
}
- (void)rightMouseDown:(NSEvent*)event {
	d_app.mouse_states[D_MOUSE_RIGHT] = D_BTN_PRESSED;
}
- (void)rightMouseUp:(NSEvent*)event {
	d_app.mouse_states[D_MOUSE_RIGHT] = D_BTN_RELEASED;
}
- (void)scrollWheel:(NSEvent*)event {
	d_app.wheel = vec2f(event.scrollingDeltaX, event.scrollingDeltaY);
}
- (void)drawRect:(NSRect)rect {

	NSPoint ompos = [d_app.window mouseLocationOutsideOfEventStream];
	vec2 mpos = vec2f(
		ompos.x * d_app.width / d_app.win_width,
		d_app.height - ompos.y * d_app.height / d_app.win_height
	);
	d_app.mouse_dpos = vec2_sub(mpos, d_app.mouse_pos);
	d_app.mouse_pos = mpos;

	d_app_frame();

#if defined(D_GL)
	d_gl_blit();
#elif defined(D_METAL)
	d_mtl_blit();
#elif defined(D_CPU)

	int w = d_width();
	int h = d_height();

	CGContextRef ctx = [[NSGraphicsContext currentContext] CGContext];
	CGContextSetInterpolationQuality(ctx, kCGInterpolationNone);
	CGColorSpaceRef rgb = CGColorSpaceCreateDeviceRGB();
	CGDataProviderRef provider = CGDataProviderCreateWithData(
		NULL,
		d_app.buf,
		w * h * 4,
		NULL
	);

	CGImageRef img = CGImageCreate(
		w,
		h,
		8,
		32,
		4 * w,
		rgb,
		kCGBitmapByteOrderDefault | kCGImageAlphaLast,
		provider,
		NULL,
		false,
		kCGRenderingIntentDefault
	);

	CGContextDrawImage(ctx, rect, img);

	CGColorSpaceRelease(rgb);
	CGDataProviderRelease(provider);
	CGImageRelease(img);

#endif // D_CPU

	if (d_app.quit) {
		[NSApp terminate:nil];
	}

}
@end

static void d_cocoa_run(const d_desc *desc) {
	[NSApplication sharedApplication];
	[NSApp setDelegate:[[DAppDelegate alloc] init]];
	[NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
	[NSApp activateIgnoringOtherApps:YES];
	[NSApp run];
}

#endif // D_COCOA

// -------------------------------------------------------------
// UIKit
#if defined(D_UIKIT)

static void d_uikit_touch(d_btn state, NSSet<UITouch*> *tset, UIEvent *event) {

	NSArray<UITouch*> *touches = [tset allObjects];

	if ([touches count] == 1) {
		UITouch *t = touches[0];
		CGPoint pos = [t locationInView:[t view]];
		d_app.mouse_states[D_MOUSE_LEFT] = state;
		d_app.mouse_pos = vec2f(
			pos.x * d_app.width / d_app.win_width,
			pos.y * d_app.height / d_app.win_height
		);
	}

	for (UITouch *touch in touches) {
		uintptr_t id = (uintptr_t)touch;
		CGPoint cpos = [touch locationInView:[touch view]];
		vec2 pos = vec2f(cpos.x, cpos.y);
		switch (state) {
			case D_BTN_PRESSED:
				if (d_app.num_touches < D_MAX_TOUCHES) {
					d_app.touches[d_app.num_touches++] = (d_touch_state) {
						.id = id,
						.pos = pos,
						.dpos = vec2f(0.0, 0.0),
						.state = D_BTN_PRESSED,
					};
				}
				break;
			case D_BTN_DOWN:
				for (int i = 0; i < d_app.num_touches; i++) {
					d_touch_state *t = &d_app.touches[i];
					if (t->id == id) {
						t->dpos = vec2_sub(pos, t->pos);
						t->pos = pos;
					}
				}
				break;
			case D_BTN_RELEASED:
				for (int i = 0; i < d_app.num_touches; i++) {
					d_touch_state *t = &d_app.touches[i];
					if (t->id == id) {
						t->state = D_BTN_RELEASED;
					}
				}
				break;
			default:
				break;
		}
	}

}

@implementation DView
- (void)drawRect:(CGRect)rect {

	d_app_frame();

#if defined(D_GL)
	d_gl_blit();
#elif defined(D_METAL)
	d_mtl_blit();
#elif defined(D_CPU)

	int w = d_width();
	int h = d_height();

	CGContextRef ctx = UIGraphicsGetCurrentContext();
	CGContextSetInterpolationQuality(ctx, kCGInterpolationNone);
	CGColorSpaceRef rgb = CGColorSpaceCreateDeviceRGB();
	CGDataProviderRef provider = CGDataProviderCreateWithData(
		NULL,
		d_app.buf,
		w * h * 4,
		NULL
	);

	CGImageRef img = CGImageCreate(
		w,
		h,
		8,
		32,
		4 * w,
		rgb,
		kCGBitmapByteOrderDefault | kCGImageAlphaLast,
		provider,
		NULL,
		false,
		kCGRenderingIntentDefault
	);

	// TODO: why is it up side down
	CGContextTranslateCTM(ctx, 0, rect.size.height);
	CGContextScaleCTM(ctx, 1.0, -1.0);
	CGContextDrawImage(ctx, rect, img);

	CGColorSpaceRelease(rgb);
	CGDataProviderRelease(provider);
	CGImageRelease(img);

#endif // D_CPU

}

- (void)touchesBegan:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
	d_uikit_touch(D_BTN_PRESSED, touches, event);
}
- (void)touchesMoved:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
	d_uikit_touch(D_BTN_DOWN, touches, event);
}
- (void)touchesEnded:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
	d_uikit_touch(D_BTN_RELEASED, touches, event);
}
- (void)touchesCancelled:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
	d_uikit_touch(D_BTN_RELEASED, touches, event);
}
@end

@implementation DAppDelegate
- (BOOL)application:(UIApplication*)app didFinishLaunchingWithOptions:(NSDictionary*)opt {

	CGRect screen_rect = [[UIScreen mainScreen] bounds];
	UIWindow *window = [[UIWindow alloc] initWithFrame:screen_rect];
	d_app.window = window;
	d_app.win_width = screen_rect.size.width;
	d_app.win_height = screen_rect.size.height;

	UIViewController *view_ctrl = [[UIViewController alloc] init];
	DView *view = [[DView alloc] init];
	d_app.view = view;
	view_ctrl.view = view;
	window.rootViewController = view_ctrl;
	[window makeKeyAndVisible];

#if defined(D_GL)
	d_gl_init();
#elif defined(D_METAL)
	d_mtl_init();
#endif

	d_app_init();

	[NSTimer
		scheduledTimerWithTimeInterval:0.001
		target:self
		selector:@selector(loop:)
		userInfo:nil
		repeats:YES
	];

	return YES;

}
-(void)loop:(NSTimer*)timer {
	[d_app.view setNeedsDisplay];
}
@end

static void d_uikit_run(const d_desc *desc) {
	UIApplicationMain(0, nil, nil, NSStringFromClass([DAppDelegate class]));
}

#endif // D_UIKIT

// -------------------------------------------------------------
// Term
#if defined(D_TERM)

static void d_term_run(const d_desc *desc) {
	// TODO
}

#endif // D_TERM

// -------------------------------------------------------------
// X11
#if defined(D_X11)

static d_key d_x11_key(unsigned short k) {
	switch (k) {
		case 0x3d: return D_KEY_ESC;
	}
	return D_KEY_NONE;
}

static void d_x11_run(const d_desc *desc) {

	Display *dis = XOpenDisplay(NULL);
	d_app.display = dis;
	int screen = XDefaultScreen(dis);
	Visual *visual = XDefaultVisual(dis, screen);
	unsigned int depth = XDefaultDepth(dis, screen);
	XSetWindowAttributes attrs;

	Window window = XCreateWindow(
		dis,
		XDefaultRootWindow(dis),
		0,
		0,
		d_app.win_width,
		d_app.win_height,
		0,
		depth,
		InputOutput,
		visual,
		CWBackPixel | CWBorderPixel | CWBackingStore,
		&attrs
	);

	d_app.window = window;
	Atom del_window = XInternAtom(dis, "WM_DELETE_WINDOW", 0);
	XSetWMProtocols(dis, window, &del_window, 1);

	XSelectInput(
		dis,
		window,
		ExposureMask
		| KeyPressMask
		| KeyReleaseMask
		| ButtonPressMask
		| ButtonReleaseMask
		| PointerMotionMask
	);
	XMapWindow(dis, window);

	if (desc->title) {
		XStoreName(dis, window, desc->title);
	}

#if defined(D_GL)

	GLint glx_attrs[] = {
		GLX_RGBA,
		GLX_DOUBLEBUFFER,
		GLX_DEPTH_SIZE,     24,
		GLX_STENCIL_SIZE,   8,
		GLX_RED_SIZE,       8,
		GLX_GREEN_SIZE,     8,
		GLX_BLUE_SIZE,      8,
		GLX_DEPTH_SIZE,     24,
		GLX_STENCIL_SIZE,   8,
		GLX_SAMPLE_BUFFERS, 0,
		GLX_SAMPLES,        0,
		None
	};

	XVisualInfo *glxvis = glXChooseVisual(dis, screen, glx_attrs);
	GLXContext glx = glXCreateContext(dis, glxvis, NULL, GL_TRUE);
	glXMakeCurrent(dis, window, glx);

	d_gl_init();

#elif defined(D_CPU)

	GC gc = XDefaultGC(dis,screen);

#endif

	d_app_init();

	while (!d_app.quit) {

		int count = XPending(dis);

		while (count--) {

			XEvent event;
			XNextEvent(dis, &event);

			switch (event.type) {
				case KeyPress: {
					d_key k = d_x11_key(event.xkey.keycode);
					if (k) {
						d_app.key_states[k] = D_BTN_PRESSED;
					}
					break;
				}
				case KeyRelease: {
					d_key k = d_x11_key(event.xkey.keycode);
					if (k) {
						d_app.key_states[k] = D_BTN_RELEASED;
					}
					break;
				}
				case ButtonPress:
					d_app.mouse_states[D_MOUSE_LEFT] = D_BTN_PRESSED;
					break;
				case ButtonRelease:
					d_app.mouse_states[D_MOUSE_LEFT] = D_BTN_RELEASED;
					break;
				case MotionNotify:
					d_app.mouse_pos = vec2f(
						event.xmotion.x * d_app.width / d_app.win_width,
						event.xmotion.y * d_app.height / d_app.win_height
					);
					break;
				case ClientMessage:
					d_app.quit = true;
					break;
			}
		}

		d_app_frame();
		usleep(16000);

		// TODO: it's drawing in BGRA
		// TODO: scale
		// TODO: better fix? this is very slow

#if defined(D_CPU)
		d_img img = d_make_img(d_app.win_width, d_app.win_height);

		for (int x = 0; x < img.width; x++) {
			for (int y = 0; y < img.height; y++) {
				int xx = x * d_app.width / img.width;
				int yy = y * d_app.height / img.height;
				int i = yy * d_app.width + xx;
				color c = d_app.buf[i];
				d_img_set(&img, x, y, colori(c.b, c.g, c.r, c.a));
			}
		}

		XImage *ximg = XCreateImage(
			dis,
			visual,
			depth,
			ZPixmap,
			0,
			(char*)img.pixels,
			img.width,
			img.height,
			32,
			0
		);

		// TODO: this is slow af
		XPutImage(dis, window, gc, ximg, 0, 0, 0, 0, img.width, img.height);
		XFree(ximg);
		d_free_img(&img);

#elif defined(D_GL)

// 		d_gl_blit();

#endif

	}

	XDestroyWindow(dis, window);
	XCloseDisplay(dis);

}

#endif // D_X11

// -------------------------------------------------------------
// Web
#if defined(D_CANVAS)

static bool streq(const char *s1, const char *s2) {
	return strcmp(s1, s2) == 0;
}

static d_key d_web_key(const char *k, int loc) {

	if      (streq(k, "a")) return D_KEY_A;
	else if (streq(k, "b")) return D_KEY_B;
	else if (streq(k, "c")) return D_KEY_C;
	else if (streq(k, "d")) return D_KEY_D;
	else if (streq(k, "e")) return D_KEY_E;
	else if (streq(k, "f")) return D_KEY_F;
	else if (streq(k, "g")) return D_KEY_G;
	else if (streq(k, "h")) return D_KEY_H;
	else if (streq(k, "i")) return D_KEY_I;
	else if (streq(k, "j")) return D_KEY_J;
	else if (streq(k, "k")) return D_KEY_K;
	else if (streq(k, "l")) return D_KEY_L;
	else if (streq(k, "m")) return D_KEY_M;
	else if (streq(k, "n")) return D_KEY_N;
	else if (streq(k, "o")) return D_KEY_O;
	else if (streq(k, "p")) return D_KEY_P;
	else if (streq(k, "q")) return D_KEY_Q;
	else if (streq(k, "r")) return D_KEY_R;
	else if (streq(k, "s")) return D_KEY_S;
	else if (streq(k, "t")) return D_KEY_T;
	else if (streq(k, "u")) return D_KEY_U;
	else if (streq(k, "v")) return D_KEY_V;
	else if (streq(k, "w")) return D_KEY_W;
	else if (streq(k, "x")) return D_KEY_X;
	else if (streq(k, "y")) return D_KEY_Y;
	else if (streq(k, "z")) return D_KEY_Z;
	else if (streq(k, "1")) return D_KEY_1;
	else if (streq(k, "2")) return D_KEY_2;
	else if (streq(k, "3")) return D_KEY_3;
	else if (streq(k, "4")) return D_KEY_4;
	else if (streq(k, "5")) return D_KEY_5;
	else if (streq(k, "6")) return D_KEY_6;
	else if (streq(k, "7")) return D_KEY_7;
	else if (streq(k, "8")) return D_KEY_8;
	else if (streq(k, "9")) return D_KEY_9;
	else if (streq(k, "0")) return D_KEY_0;
	else if (streq(k, "-")) return D_KEY_MINUS;
	else if (streq(k, "=")) return D_KEY_EQUAL;
	else if (streq(k, " ")) return D_KEY_SPACE;
	else if (streq(k, ",")) return D_KEY_COMMA;
	else if (streq(k, ".")) return D_KEY_PERIOD;
	else if (streq(k, "/")) return D_KEY_SLASH;
	else if (streq(k, "[")) return D_KEY_LBRACKET;
	else if (streq(k, "]")) return D_KEY_RBRACKET;
	else if (streq(k, "\\")) return D_KEY_BACKSLASH;
	else if (streq(k, ";")) return D_KEY_SEMICOLON;
	else if (streq(k, "Enter")) return D_KEY_ENTER;
	else if (streq(k, "Escape")) return D_KEY_ESC;
	else if (streq(k, "Backspace")) return D_KEY_BACKSPACE;
	else if (streq(k, "Tab")) return D_KEY_TAB;
	else if (streq(k, "'")) return D_KEY_QUOTE;
	else if (streq(k, "`")) return D_KEY_BACKQUOTE;
	else if (streq(k, "F1")) return D_KEY_F1;
	else if (streq(k, "F2")) return D_KEY_F2;
	else if (streq(k, "F3")) return D_KEY_F3;
	else if (streq(k, "F4")) return D_KEY_F4;
	else if (streq(k, "F5")) return D_KEY_F5;
	else if (streq(k, "F6")) return D_KEY_F6;
	else if (streq(k, "F7")) return D_KEY_F7;
	else if (streq(k, "F5")) return D_KEY_F5;
	else if (streq(k, "F6")) return D_KEY_F6;
	else if (streq(k, "F7")) return D_KEY_F7;
	else if (streq(k, "F8")) return D_KEY_F8;
	else if (streq(k, "F9")) return D_KEY_F9;
	else if (streq(k, "F10")) return D_KEY_F10;
	else if (streq(k, "F11")) return D_KEY_F11;
	else if (streq(k, "F12")) return D_KEY_F12;
	else if (streq(k, "ArrowRight")) return D_KEY_RIGHT;
	else if (streq(k, "ArrowLeft")) return D_KEY_LEFT;
	else if (streq(k, "ArrowDown")) return D_KEY_DOWN;
	else if (streq(k, "ArrowUp")) return D_KEY_UP;
	else if (streq(k, "Control")) return D_KEY_RCTRL;

	if (loc == 1) {
		if      (streq(k, "Alt")) return D_KEY_LALT;
		else if (streq(k, "Meta")) return D_KEY_LMETA;
		else if (streq(k, "Control")) return D_KEY_LCTRL;
		else if (streq(k, "Shift")) return D_KEY_LSHIFT;
	} else if (loc == 2) {
		if      (streq(k, "Alt")) return D_KEY_RALT;
		else if (streq(k, "Meta")) return D_KEY_RMETA;
		else if (streq(k, "Control")) return D_KEY_RCTRL;
		else if (streq(k, "Shift")) return D_KEY_RSHIFT;
	}

	return D_KEY_NONE;
}

EMSCRIPTEN_KEEPALIVE void d_cjs_set_win_size(int w, int h) {
	d_app.win_width = w;
	d_app.win_height = h;
}

EMSCRIPTEN_KEEPALIVE void d_cjs_set_mouse_pos(float x, float y) {
	d_app.mouse_pos = vec2f(
		x * d_app.width / d_app.win_width,
		y * d_app.height / d_app.win_height
	);
}

EMSCRIPTEN_KEEPALIVE void d_cjs_key_press(const char *key, int loc, bool rep) {
	d_key k = d_web_key(key, loc);
	if (!k) {
		return;
	}
	if (rep) {
		d_app.key_states[k] = D_BTN_RPRESSED;
	} else {
		d_app.key_states[k] = D_BTN_PRESSED;
	}
}

EMSCRIPTEN_KEEPALIVE void d_cjs_key_release(const char *key, int loc) {
	d_key k = d_web_key(key, loc);
	if (!k) {
		return;
	}
	d_app.key_states[k] = D_BTN_RELEASED;
}

EMSCRIPTEN_KEEPALIVE void d_cjs_mouse_press() {
	d_app.mouse_states[D_MOUSE_LEFT] = D_BTN_PRESSED;
}

EMSCRIPTEN_KEEPALIVE void d_cjs_mouse_release() {
	d_app.mouse_states[D_MOUSE_LEFT] = D_BTN_RELEASED;
}

EM_JS(void, d_js_set_fullscreen, (bool b), {
	const canvas = dirty.canvas;
	if (b) {
		// TODO: becomes blurry
		dirty.originWidth = canvas.width;
		dirty.originHeight = canvas.height;
		canvas.width = window.innerWidth;
		canvas.height = window.innerHeight;
		dirty.fullscreen = true;
	} else {
		canvas.width = dirty.originWidth || canvas.width;
		canvas.height = dirty.originHeight || canvas.height;
		dirty.fullscreen = false;
	}
})

EM_JS(bool, d_js_is_fullscreen, (), {
	return dirty.fullscreen;
})

EM_JS(bool, d_js_set_title, (const char *title), {
	document.title = UTF8ToString(title);
})

EM_JS(void, d_js_canvas_init, (const char *root, int w, int h), {

	window.dirty = {};

	const name = UTF8ToString(root);
	const canvas = document.createElement("canvas");
	dirty.canvas = canvas;
	canvas.width = w;
	canvas.height = h;
	canvas.id = "dirty";
	canvas.style = "image-rendering: pixelated; image-rendering: crisp-edges;";

	if (name) {
		document.getElementById(name).appendChild(canvas);
	} else {
		document.body.appendChild(canvas);
	}

#if defined(D_CPU)

	const ctx = canvas.getContext("2d");

	ctx.imageSmoothingEnabled = false;

#elif defined(D_GL)

	const gl = canvas.getContext("webgl");

	gl.clearColor(0.0, 0.0, 0.0, 0.0);
	gl.clear(gl.COLOR_BUFFER_BIT);

#endif

	const preventDefaultKeys = [
		" ",
		"ArrowLeft",
		"ArrowRight",
		"ArrowUp",
		"ArrowDown",
		"Backspace",
	];

	document.addEventListener("mousemove", (e) => {
		const rect = canvas.getBoundingClientRect();
		_d_cjs_set_mouse_pos(e.pageX - rect.left, e.pageY - rect.top);
	});

	document.addEventListener("keydown", (e) => {
		if (preventDefaultKeys.includes(e.key)) {
			e.preventDefault();
		}
		ccall(
			'd_cjs_key_press',
			'void',
			[ 'string', 'number', 'bool' ],
			[ e.key, e.location, e.repeat ]
		);
	});

	document.addEventListener("keyup", (e) => {
		ccall(
			'd_cjs_key_release',
			'void',
			[ 'string', 'number' ],
			[ e.key, e.location ]
		);
	});

	document.addEventListener("mousedown", (e) => {
		_d_cjs_mouse_press();
	});

	document.addEventListener("mouseup", (e) => {
		_d_cjs_mouse_release();
	});

})

EM_JS(void, d_js_canvas_frame, (const color *buf, int w, int h), {

	dirty.width = w;
	dirty.height = h;

	const canvas = dirty.canvas;
	const pixels = new Uint8ClampedArray(HEAPU8.buffer, buf, w * h * 4);
	const img = new ImageData(pixels, w, h);

	_d_cjs_set_win_size(canvas.width, canvas.height);

#if defined(D_CPU)

	const ctx = canvas.getContext("2d");

	ctx.putImageData(img, 0, 0);
	ctx.setTransform(canvas.width / w, 0, 0, canvas.height / h, 0, 0);
	ctx.drawImage(canvas, 0, 0);

#elif defined(D_GL)

	const gl = canvas.getContext("webgl");
	// TODO
#endif

})

static void d_canvas_loop() {
	d_app_frame();
	d_js_canvas_frame(d_app.buf, d_app.width, d_app.height);
}

static void d_canvas_run(const d_desc *desc) {
	d_js_canvas_init(desc->canvas_root, d_app.win_width, d_app.win_height);
	d_app_init();
	emscripten_set_main_loop(d_canvas_loop, 0, true);
}

#endif // D_CANVAS

void d_run(d_desc desc) {

	d_app.desc = desc;
	float scale = desc.scale ? desc.scale : 1.0;
	d_app.width = desc.width ? desc.width : 256;
	d_app.height = desc.height ? desc.height : 256;
	d_app.win_width = d_app.width * scale;
	d_app.win_height = d_app.height * scale;

#if defined(D_COCOA)
	d_cocoa_run(&desc);
#elif defined(D_UIKIT)
	d_uikit_run(&desc);
#elif defined(D_X11)
	d_x11_run(&desc);
#elif defined(D_CANVAS)
	d_canvas_run(&desc);
#elif defined(D_TERM)
	d_term_run(&desc);
#endif

}

void d_quit() {
	d_app.quit = true;
}

// TODO: use macros to reduce repeat?
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

float d_time() {
	return d_app.time;
}

float d_dt() {
	return d_app.dt;
}

int d_fps() {
	return d_app.fps;
}

void d_set_fullscreen(bool b) {
#if defined(D_COCOA)
	if (b != d_fullscreen()) {
		[d_app.window toggleFullScreen:nil];
	}
#elif defined(D_X11)
	// TODO
#elif defined(D_CANVAS)
	d_js_set_fullscreen(b);
#endif
}

bool d_fullscreen() {
#if defined(D_COCOA)
	return [d_app.window styleMask] & NSWindowStyleMaskFullScreen;
#elif defined(D_UIKIT)
	return true;
#elif defined(D_CANVAS)
	return d_js_is_fullscreen();
#endif
	return false;
}

// TODO
void d_lock_mouse(bool b) {
}

// TODO
bool d_mouse_locked() {
	return false;
}

// TODO
void d_hide_mouse(bool b) {
}

// TODO
bool d_mouse_hidden() {
	return false;
}

void d_set_title(const char *title) {
#if defined(D_COCOA)
	@autoreleasepool {
		[d_app.window setTitle:[NSString stringWithUTF8String:title]];
	}
#elif defined(D_X11)
	XStoreName(d_app.display, d_app.window, title);
#elif defined(D_CANVAS)
	d_js_set_title(title);
#endif
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
		case D_KMOD_ALT:
			return d_key_down(D_KEY_LALT) || d_key_down(D_KEY_RALT);
		case D_KMOD_META:
			return d_key_down(D_KEY_LMETA) || d_key_down(D_KEY_RMETA);
		case D_KMOD_CTRL:
			return d_key_down(D_KEY_LCTRL) || d_key_down(D_KEY_RCTRL);
		case D_KMOD_SHIFT:
			return d_key_down(D_KEY_LSHIFT) || d_key_down(D_KEY_RSHIFT);
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
	return d_app.mouse_states[k] == D_BTN_DOWN
		|| d_app.mouse_states[k] == D_BTN_PRESSED;
}

int d_width() {
	return d_app.width;
}

int d_height() {
	return d_app.height;
}

int d_win_width() {
	return d_app.win_width;
}

int d_win_height() {
	return d_app.win_height;
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
	d_assert(t < D_MAX_TOUCHES, "touch not found: %d\n", t);
	return d_app.touches[t].state == D_BTN_PRESSED;
}

bool d_touch_released(d_touch t) {
	d_assert(t < D_MAX_TOUCHES, "touch not found: %d\n", t);
	return d_app.touches[t].state == D_BTN_RELEASED;
}

bool d_touch_moved(d_touch t) {
	d_assert(t < D_MAX_TOUCHES, "touch not found: %d\n", t);
	return d_app.touches[t].dpos.x != 0.0 || d_app.touches[t].dpos.x != 0.0;
}

vec2 d_touch_pos(d_touch t) {
	d_assert(t < D_MAX_TOUCHES, "touch not found: %d\n", t);
	return d_app.touches[t].pos;
}

vec2 d_touch_dpos(d_touch t) {
	d_assert(t < D_MAX_TOUCHES, "touch not found: %d\n", t);
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

bool d_active() {
#if defined(D_COCOA)
	return [d_app.window isMainWindow];
#endif
	return true;
}

// gfx                    ~                       o                   ~
//         ~         .                 ~          .          O
//   .         _              O                              o
//        .  ><_'>            o           .         .        .
//                            .               .                    .
//  .                   ~          .                  ~
//              o           ~               ~                   .
//      O       .               .        _                  O
//      o                .             <'_><        .       o         .
//      .    .                                              .
//                ~         .     o             ~                   .
//   ~               .            .          .                 ~

typedef struct {
	d_img def_canvas;
	d_img *cur_canvas;
	d_font def_font;
	d_font *cur_font;
	color clear_color;
	d_blend blend;
	d_wrap wrap;
} d_gfx_ctx;

static d_gfx_ctx d_gfx;

void d_gfx_init(const d_desc *desc) {
	d_gfx.def_canvas = d_make_img(d_width(), d_height());
	d_gfx.cur_canvas = &d_gfx.def_canvas;
	d_gfx.def_font = d_parse_font(unscii_bytes);
	d_gfx.cur_font = &d_gfx.def_font;
	d_gfx.blend = D_ALPHA;
	d_gfx.wrap = D_BORDER;
	d_gfx.clear_color = desc->clear_color;
	d_clear();
}

void d_gfx_frame_end() {
	d_present(d_gfx.cur_canvas->pixels);
	d_gfx.cur_canvas = &d_gfx.def_canvas;
}

#define D_IMG_HEADER_SIZE \
	sizeof(uint16_t) \
	+ sizeof(uint16_t) \
	+ sizeof(uint8_t) \
	+ sizeof(uint8_t) \

typedef struct {
	uint16_t width;
	uint16_t height;
	uint8_t num_frames;
	uint8_t num_anims;
	uint8_t *pixels;
} d_img_bin;

d_img d_make_img(int w, int h) {
	return (d_img) {
		.width = w,
		.height = h,
		.pixels = calloc(w * h, sizeof(color)),
	};
}

d_img d_parse_img(const uint8_t *bytes) {

	d_img_bin *data = (d_img_bin*)bytes;
	int size = sizeof(uint8_t) * data->width * data->height * 4;
	uint8_t *pixels = malloc(size);
	memcpy(pixels, (uint8_t*)(bytes + D_IMG_HEADER_SIZE), size);

	return (d_img) {
		.width = data->width,
		.height = data->height,
		.pixels = (color*)pixels,
	};

}

d_img d_load_img(const char *path) {
	size_t size;
	uint8_t *bytes = d_read_bytes(path, &size);
	d_img img = d_parse_img(bytes);
	free(bytes);
	return img;
}

void d_img_set_ex(d_img *img, int x, int y, color c, d_blend blend) {
	if (x < 0 || x >= img->width || y < 0 || y >= img->height) {
		return;
	}
	int i = (int)(y * img->width + x);
	switch (blend) {
		case D_ALPHA: {
			if (c.a == 255) {
				img->pixels[i] = c;
			} else if (c.a != 0) {
				color rc = img->pixels[i];
				img->pixels[i] = (color) {
					.r = (rc.r * (255 - c.a) + c.r * c.a) / 255,
					.g = (rc.g * (255 - c.a) + c.g * c.a) / 255,
					.b = (rc.b * (255 - c.a) + c.b * c.a) / 255,
					.a = (rc.a * (255 - c.a) + c.a * c.a) / 255,
				};
			}
			break;
		}
		case D_REPLACE:
			img->pixels[i] = c;
			break;
		case D_ADD:
			if (c.a != 0) {
				color rc = img->pixels[i];
				img->pixels[i] = (color) {
					.r = (rc.r * rc.a + c.r * c.a) / 255,
					.g = (rc.g * rc.a + c.g * c.a) / 255,
					.b = (rc.b * rc.a + c.b * c.a) / 255,
					.a = (rc.a * rc.a + c.a * c.a) / 255,
				};
			}
			break;
	}
}

void d_img_set(d_img *img, int x, int y, color c) {
	d_img_set_ex(img, x, y, c, D_REPLACE);
}

color d_img_get_ex(const d_img *img, int x, int y, d_wrap wrap) {
	switch (wrap) {
		case D_BORDER:
			if (x < 0 || x >= img->width || y < 0 || y >= img->height) {
				return colori(0, 0, 0, 0);
			} else {
				return img->pixels[(int)(y * img->width + x)];
			}
		case D_EDGE: {
			int xx = clampi(x, 0, img->width - 1);
			int yy = clampi(y, 0, img->height - 1);
			return img->pixels[(int)(yy * img->width + xx)];
		}
		case D_REPEAT: {
			int xx = x % img->width;
			int yy = y % img->height;
			return img->pixels[(int)(yy * img->width + xx)];
		}
	}
}

color d_img_get(const d_img *img, int x, int y) {
	return d_img_get_ex(img, x, y, D_BORDER);
}

void d_img_fill(d_img *img, color c) {
	if (c.r == 0 && c.g == 0 && c.b == 0 && c.a == 0) {
		memset(img->pixels, 0, sizeof(color) * img->width * img->height);
	} else {
		for (int i = 0; i < img->width * img->height; i++) {
			img->pixels[i] = c;
		}
	}
}

void d_img_save(const d_img *img, const char *path) {

	d_img_bin data = (d_img_bin) {
		.width = img->width,
		.height = img->height,
		.num_frames = 1,
		.num_anims = 0,
		.pixels = (uint8_t*)img->pixels,
	};

	FILE *f = fopen(path, "wb");
	fwrite(&data, D_IMG_HEADER_SIZE, 1, f);
	fwrite(data.pixels, sizeof(uint8_t), img->width * img->height * 4, f);
	fclose(f);

}

d_img d_img_clone(const d_img *img) {
	d_img img2 = d_make_img(img->width, img->height);
	memcpy(img2.pixels, img->pixels, img->width * img->height * sizeof(color));
	return img2;
}

void d_free_img(d_img *img) {
	free(img->pixels);
	img->pixels = NULL;
	img->width = 0;
	img->height = 0;
}

#define D_FNT_HEADER_SIZE \
	sizeof(uint8_t) \
	+ sizeof(uint8_t) \
	+ sizeof(uint8_t) \
	+ sizeof(uint8_t) \
	+ sizeof(char) * 128 \

typedef struct {
	uint8_t gw;
	uint8_t gh;
	uint8_t cols;
	uint8_t rows;
	char chars[128];
	uint8_t *pixels;
} d_fnt_bin;

d_font d_parse_font(const uint8_t *bytes) {

	d_fnt_bin *data = (d_fnt_bin*)bytes;
	int size = sizeof(uint8_t) * data->gw * data->gh * data->cols * data->rows;
	uint8_t *pixels = malloc(size);
	memcpy(pixels, (uint8_t*)(bytes + D_FNT_HEADER_SIZE), size);

	d_font f = {0};

	f.gw = data->gw;
	f.gh = data->gh;
	f.rows = data->rows;
	f.cols = data->cols;
	f.pixels = pixels;

	int num_chars = f.cols * f.rows;

	for (int i = 0; i < num_chars; i++) {
		f.map[(int)data->chars[i]] = i;
	}

	return f;

}

void d_free_font(d_font *f) {
	free(f->pixels);
	f->pixels = NULL;
}

void d_free_mesh(d_mesh *m) {
	free(m->verts);
	m->num_verts = 0;
	free(m->indices);
	m->num_indices = 0;
}

void d_clear() {
	d_img_fill(d_gfx.cur_canvas, d_gfx.clear_color);
}

void d_put(int x, int y, color c) {
	d_img_set_ex(d_gfx.cur_canvas, x, y, c, d_gfx.blend);
}

color d_peek(int x, int y) {
	return d_img_get_ex(d_gfx.cur_canvas, x, y, d_gfx.wrap);
}

void d_draw_img(const d_img *img, vec2 pos) {
	for (int x = 0; x < img->width; x++) {
		for (int y = 0; y < img->height; y++) {
			d_put(x + pos.x, y + pos.y, img->pixels[y * img->width + x]);
		}
	}
}

void d_draw_tri(vec2 p1, vec2 p2, vec2 p3, color c) {
	// TODO
}

void d_draw_rect(vec2 p1, vec2 p2, color c) {

	int x1 = p1.x < p2.x ? p1.x : p2.x;
	int x2 = p1.x > p2.x ? p1.x : p2.x;
	int y1 = p1.y < p2.y ? p1.y : p2.y;
	int y2 = p1.y > p2.y ? p1.y : p2.y;

	for (int i = x1; i < x2; i++) {
		for (int j = y1; j < y2; j++) {
			d_put(i, j, c);
		}
	}

}

void d_draw_circle(vec2 center, float r, color c) {

	for (int i = center.x - r; i <= center.x + r; i++) {
		for (int j = center.y - r; j <= center.y + r; j++) {
			vec2 p = vec2f(i, j);
			float d = vec2_dist(p, center);
			if (d <= r) {
				d_put(p.x, p.y, c);
			}
		}
	}

}

void d_draw_text(const char *text, vec2 pos) {

	int num_chars = strlen(text);
	d_font *font = &d_gfx.def_font;
	int rw = font->gw * font->cols;
	int ox = 0;

	for (int i = 0; i < num_chars; i++) {

		int ii = font->map[(int)text[i]];
		int xx = ii % font->cols * font->gw;
		int yy = ii / font->cols * font->gh;

		for (int x = 0; x < font->gw; x++) {
			for (int y = 0; y < font->gh; y++) {
				if (font->pixels[(yy + y) * rw + xx + x] == 1) {
					d_put(pos.x + ox + x, pos.y + y, colori(255, 255, 255, 255));
				}
			}
		}

		ox += font->gw;

	}

}

void d_draw_line(vec2 p1, vec2 p2, color c) {

	int dx = p2.x - p1.x;
	int dy = p2.y - p1.y;
	int adx = abs(dx);
	int ady = abs(dy);
	int eps = 0;
	int sx = dx > 0 ? 1 : -1;
	int sy = dy > 0 ? 1 : -1;

	if (adx > ady) {
		for(int x = p1.x, y = p1.y; sx < 0 ? x >= p2.x : x <= p2.x; x += sx) {
			d_put(x, y, c);
			eps += ady;
			if ((eps << 1) >= adx) {
				y += sy;
				eps -= adx;
			}
		}
	} else {
		for(int x = p1.x, y = p1.y; sy < 0 ? y >= p2.y : y <= p2.y; y += sy) {
			d_put(x, y, c);
			eps += adx;
			if ((eps << 1) >= ady) {
				x += sx;
				eps -= ady;
			}
		}
	}

}

void d_draw_mesh(const d_mesh *mesh) {
	// TODO
}

void d_draw_model(const d_model *model) {
	// TODO
}

void d_set_blend(d_blend b) {
	d_gfx.blend = b;
}

void d_set_wrap(d_wrap w) {
	d_gfx.wrap = w;
}

void d_drawon(d_img *img) {
	if (img) {
		d_gfx.cur_canvas = img;
	} else {
		d_gfx.cur_canvas = &d_gfx.def_canvas;
	}
}

d_img *d_canvas() {
	return d_gfx.cur_canvas;
}

// audio                  ~                       o                   ~
//         ~         .                 ~          .          O
//   .         _              O                              o
//        .  ><_'>            o           .         .        .
//                            .               .                    .
//  .                   ~          .                  ~
//              o           ~               ~                   .
//      O       .               .        _                  O
//      o                .             <'_><        .       o         .
//      .    .                                              .
//                ~         .     o             ~                   .
//   ~               .            .          .                 ~

#include <limits.h>
#include <math.h>

#if defined(D_MACOS) || defined(D_IOS)
	#define D_COREAUDIO
#elif defined(D_LINUX)
	#define D_ALSA
#elif defined(D_ANDROID)
	#define D_AAUDIO
#elif defined(D_WEB)
	#define D_WEBAUDIO
#endif

#if defined(D_COREAUDIO)
	#include <AudioToolbox/AudioToolbox.h>
#elif defined(D_ALSA)
#elif defined(D_WEBAUDIO)
	#include <emscripten/emscripten.h>
#endif

#define D_SAMPLE_RATE 44100
#define D_NUM_CHANNELS 1
#define D_BUFFER_FRAMES 1024
#define D_MAX_PLAYBACKS 256
#define D_A4_FREQ 440
#define D_A4_NOTE 69
#define D_SYNTH_NOTES 128
#define D_SYNTH_BUF_SIZE 44100

typedef struct {
	d_voice notes[D_SYNTH_NOTES];
	float volume;
	int sample_rate;
	float clock;
	d_envelope envelope;
	float buf[D_SYNTH_BUF_SIZE];
	int buf_head;
	int buf_size;
	float (*wav_func)(float freq, float t);
} d_synth;

d_synth d_make_synth();
float d_synth_next();

typedef struct {
	d_playback playbacks[D_MAX_PLAYBACKS];
	int num_playbacks;
	float volume;
	float (*user_stream)();
	d_synth synth;
} d_audio_ctx;

static d_audio_ctx d_audio;

static float d_audio_next() {

	float frame = 0.0;

	for (int j = 0; j < d_audio.num_playbacks; j++) {

		d_playback *p = &d_audio.playbacks[j];

		if (p->paused || p->done) {
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

		frame += (float)p->src->frames[p->pos] / SHRT_MAX * p->volume;
		p->pos++;

	}

	frame += d_synth_next();

	if (d_audio.user_stream) {
		frame += d_audio.user_stream();
	}

	return frame * d_audio.volume;

}

#if defined(D_COREAUDIO)

static void d_ca_stream(void *udata, AudioQueueRef queue, AudioQueueBufferRef buffer) {

	int num_frames = buffer->mAudioDataByteSize / (sizeof(float) * D_NUM_CHANNELS);
	float *data = (float*)buffer->mAudioData;

	for (int i = 0; i < num_frames; i++) {
		data[i] = d_audio_next();
	}

	AudioQueueEnqueueBuffer(queue, buffer, 0, NULL);

}

static void d_ca_init() {

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

}

#endif // D_COREAUDIO

#if defined(D_WEBAUDIO)

EMSCRIPTEN_KEEPALIVE float d_cjs_audio_next() {
	return d_audio_next();
}

EM_JS(void, d_js_webaudio_init, (), {

	const ctx = new (window.AudioContext || window.webkitAudioContext)({
		sampleRate: 44100,
	});

	dirty.audioCtx = ctx;

// 	const buf = ctx.createBuffer(1, 1024, 44100);

// 	const loop = () => {

// 		for (let i = 0; i < buf.numberOfChannels; i++) {
// 			const dest = buf.getChannelData(i);
// 			for (let j = 0; j < dest.length; j++) {
// 				dest[j] = _d_cjs_audio_next();
// 			}
// 		}

// 		const src = ctx.createBufferSource();

// 		src.buffer = buf;
// 		src.onended = loop;
// 		src.connect(ctx.destination);
// 		src.start();

// 	};

// 	loop();

// 	const processor = ctx.createScriptProcessor(1024, 0, 1);

// 	processor.onaudioprocess = (e) => {
// 		var num_frames = e.outputBuffer.length;
// 		var num_channels = e.outputBuffer.numberOfChannels;
// 		for (let chn = 0; chn < num_channels; chn++) {
// 			let chan = e.outputBuffer.getChannelData(chn);
// 			for (let i = 0; i < num_frames; i++) {
// 				chan[i] = _d_cjs_audio_next();
// 			}
// 		}
// 	};

// 	processor.connect(ctx.destination);

})

static void d_webaudio_init() {
	d_js_webaudio_init();
}

#endif

#if defined(D_ALSA)

static void d_alsa_init() {
// 	snd_pcm_t *dev = NULL;
// 	snd_pcm_open(dev, "default", SND_PCM_STREAM_PLAYBACK, 0);
	// TODO
}

#endif // D_ALSA

void d_audio_init(const d_desc *desc) {
	d_audio.volume = 1.0;
	d_audio.user_stream = desc->stream;
	d_audio.synth = d_make_synth();
#if defined(D_COREAUDIO)
	d_ca_init();
#elif defined(D_WEBAUDIO)
	d_webaudio_init();
#elif defined(D_ALSA)
	d_alsa_init();
#endif
}

#define D_SND_HEADER_SIZE \
	sizeof(uint32_t) \

typedef struct {
	uint32_t num_frames;
	int16_t *frames;
} d_snd_bin;

d_sound d_make_sound(const short *frames, int num_frames) {
	int size = sizeof(short) * num_frames;
	short *frames_n = malloc(size);
	memcpy(frames_n, frames, size);
	return (d_sound) {
		.frames = frames_n,
		.num_frames = num_frames,
	};
}

d_sound d_parse_sound(const uint8_t *bytes) {

	d_snd_bin *data = (d_snd_bin*)bytes;
	int size = sizeof(int16_t) * data->num_frames;
	int16_t *frames = malloc(size);
	memcpy(frames, (int16_t*)(bytes + D_SND_HEADER_SIZE), size);

	return (d_sound) {
		.frames = frames,
		.num_frames = data->num_frames,
	};

}

d_sound d_load_sound(const char *path) {

	size_t size;
	uint8_t *bytes = d_read_bytes(path, &size);
	d_sound snd = d_parse_sound(bytes);
	free(bytes);

	return snd;

}

float d_sound_sample(const d_sound *snd, float time) {
	int pos = clampi(time * D_SAMPLE_RATE, 0, snd->num_frames - 1);
	return (float)snd->frames[pos] / SHRT_MAX;
}

float d_sound_len(const d_sound *snd) {
	return (float)snd->num_frames / (float)D_SAMPLE_RATE;
}

void d_free_sound(d_sound *snd) {
	free(snd->frames);
	snd->frames = NULL;
}

d_playback *d_play(const d_sound *snd) {
	return d_play_ex(snd, (d_play_conf) {
		.loop = false,
		.paused = false,
		.volume = 1.0,
	});
}

d_playback *d_play_ex(const d_sound *snd, d_play_conf conf) {

	int pos = clampi((int)(conf.time * D_SAMPLE_RATE), 0, snd->num_frames - 1);

	d_playback src = (d_playback) {
		.src = snd,
		.pos = pos,
		.loop = conf.loop,
		.paused = conf.paused,
		.volume = conf.volume,
		.done = false,
	};

	for (int i = 0; i < d_audio.num_playbacks; i++) {
		if (d_audio.playbacks[i].done) {
			d_audio.playbacks[i] = src;
			return &d_audio.playbacks[i];
		}
	}

	d_audio.playbacks[d_audio.num_playbacks] = src;

	return &d_audio.playbacks[d_audio.num_playbacks++];

}

void d_playback_seek(d_playback *pb, float time) {
	pb->pos = clampi(time * D_SAMPLE_RATE, 0, pb->src->num_frames - 1);
}

float d_playback_time(d_playback *pb) {
	return (float)pb->pos / (float)D_SAMPLE_RATE;
}

float d_note_freq(int n) {
	return D_A4_FREQ * pow(powf(2.0, 1.0 / 12.0), n - D_A4_NOTE);
}

float d_wav_sin(float freq, float t) {
	return sin(freq * 2.0 * D_PI * t);
}

float d_wav_square(float freq, float t) {
	return d_wav_sin(freq, t) > 0.0 ? 1.0 : -1.0;
}

float d_wav_tri(float freq, float t) {
	return asin(d_wav_sin(freq, t)) * 2.0 / D_PI;
}

float d_wav_saw(float freq, float t) {
	return (2.0 / D_PI) * (freq * D_PI * fmod(t, 1.0 / freq) - D_PI / 2.0);
}

float d_wav_noise(float freq, float t) {
	return randf(-1.0, 1.0);
}

d_synth d_make_synth() {
	return (d_synth) {
		.notes = {0},
		.volume = 0.5,
		.clock = 0,
		.sample_rate = D_SAMPLE_RATE,
		.wav_func = d_wav_sin,
		.envelope = (d_envelope) {
			.attack = 0.05,
			.decay = 0.05,
			.sustain = 1.0,
			.release = 0.5,
		},
	};
}

d_voice d_make_voice() {
	return (d_voice) {
		.active = true,
		.life = 0.0,
		.afterlife = 0.0,
		.volume = 0.0,
		.alive = true,
	};
}

void d_synth_play(int note) {
	d_assert(note >= 0 && note < D_SYNTH_NOTES, "note out of bound: '%d'\n", note);
	d_audio.synth.notes[note] = d_make_voice();
}

void d_synth_release(int note) {
	d_assert(note >= 0 && note < D_SYNTH_NOTES, "note out of bound: '%d'\n", note);
	d_audio.synth.notes[note].active = false;
}

void d_voice_process(d_voice *v, const d_envelope *e, float dt) {

	if (!v->alive) {
		return;
	}

	float a = e->attack;
	float d = e->decay;
	float s = e->sustain;
	float r = e->release;

	// attack
	if (v->life <= a) {
		if (a == 0.0) {
			v->volume = 1.0;
		} else {
			v->volume = v->life / a;
		}
	// decay
	} else if (v->life > a && v->life <= a + d) {
		v->volume = 1.0 - (v->life - a) / d * (1.0 - s);
	// systain
	} else {
		if (v->active) {
			v->volume = s;
		// release
		} else {
			if (r == 0.0) {
				v->volume = 0.0;
			} else {
				v->volume = s * (1.0 - (v->afterlife / r));
				if (v->volume <= 0.0) {
					v->alive = false;
				}
			}
		}
	}

	v->life += dt;

	if (!v->active) {
		v->afterlife += dt;
	}

}

float d_synth_next() {

	d_synth *synth = &d_audio.synth;
	float frame = 0.0;
	float dt = 1.0 / (float)synth->sample_rate;

	synth->clock += dt;

	for (int i = 0; i < D_SYNTH_NOTES; i++) {

		d_voice *v = &synth->notes[i];

		d_voice_process(v, &synth->envelope, dt);

		float freq = d_note_freq(i);
		float sample = synth->wav_func(freq, synth->clock) * v->volume;

		frame += sample;

	}

	frame *= synth->volume;

	if (synth->buf_size < D_SYNTH_BUF_SIZE) {
		synth->buf[synth->buf_size++] = frame;
	} else {
		synth->buf[synth->buf_head++] = frame;
		if (synth->buf_head >= D_SYNTH_BUF_SIZE) {
			synth->buf_head = 0;
		}
	}

	return frame;

}

float d_synth_peek(int n) {
	d_synth *synth = &d_audio.synth;
	if (synth->buf_size == 0) {
		return 0.0;
	}
	int idx = (n + synth->buf_size - 1 + synth->buf_head) % D_SYNTH_BUF_SIZE;
	if (idx < 0 || idx >= D_SYNTH_BUF_SIZE) {
		return 0.0;
	}
	return synth->buf[idx];
}

d_envelope *d_synth_envelope() {
	return &d_audio.synth.envelope;
}

void d_synth_wav(float (*func)(float freq, float t)) {
	d_audio.synth.wav_func = func;
}

// fs                     ~                       o                   ~
//         ~         .                 ~          .          O
//   .         _              O                              o
//        .  ><_'>            o           .         .        .
//                            .               .                    .
//  .                   ~          .                  ~
//              o           ~               ~                   .
//      O       .               .        _                  O
//      o                .             <'_><        .       o         .
//      .    .                                              .
//                ~         .     o             ~                   .
//   ~               .            .          .                 ~

#include <dirent.h>
#include <sys/stat.h>

#if defined(D_MACOS) || defined(D_IOS)
	#import <Foundation/Foundation.h>
#endif

#define D_PATH_MAX 256

typedef struct {
	char res_path[D_PATH_MAX];
	char data_path[D_PATH_MAX];
} d_fs_ctx;

static d_fs_ctx d_fs;

void d_fs_init(const d_desc *desc) {

	if (desc->path) {
		sprintf(d_fs.res_path, "%s/", desc->path);
	} else {
#if defined(D_MACOS) || defined(D_IOS)
		@autoreleasepool {
			const char *res_path = [[[NSBundle mainBundle] resourcePath] UTF8String];
			sprintf(d_fs.res_path, "%s/", res_path);
		}
#elif defined(D_WINDOWS)
		// TODO
#elif defined(D_LINUX)
		// TODO
#endif
	}

	if (desc->org && desc->name) {
#if defined(D_MACOS) || defined(D_IOS)
		const char *home = getenv("HOME");
		if (home) {
			sprintf(d_fs.data_path, "%s/Library/Application Support", home);
		}
#elif defined(D_WINDOWS)
		// TODO
#elif defined(D_LINUX)
		const char *home = getenv("HOME");
		const char *xdg_data = getenv("XDG_DATA_HOME");
		if (xdg_data) {
			sprintf(d_fs.data_path, "%s", xdg_data);
		} else if (home) {
			sprintf(d_fs.data_path, "%s/.local/share", home);
		}
#endif
		sprintf(d_fs.data_path, "%s/%s", d_fs.data_path, desc->org);
		mkdir(d_fs.data_path, 0700);
		sprintf(d_fs.data_path, "%s/%s/", d_fs.data_path, desc->name);
		mkdir(d_fs.data_path, 0700);
	}

}

static char *read_text(const char *path) {

	FILE *file = fopen(path, "r");

	if (!file) {
		d_fail("failed to read: '%s'\n", path);
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *buffer = malloc(size + 1);
	size_t r_size = fread(buffer, 1, size, file);

	buffer[size] = '\0';

	if (r_size != size) {
		free(buffer);
		d_fail("failed to read: '%s'\n", path);
		return NULL;
	}

	fclose(file);

	return buffer;

}

static uint8_t *read_bytes(const char *path, size_t *osize) {

	FILE *file = fopen(path, "rb");

	if (!file) {
		d_fail("failed to read: '%s'\n", path);
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);

	uint8_t *buffer = malloc(size);
	size_t r_size = fread(buffer, 1, size, file);

	if (r_size != size) {
		free(buffer);
		d_fail("failed to read: '%s'\n", path);
		return NULL;
	}

	if (osize) {
		*osize = size;
	}

	fclose(file);

	return buffer;

}

static bool write_text(const char *path, const char *text) {

	FILE *file = fopen(path, "w");

	if (!file) {
		d_fail("failed to write: '%s'\n", path);
		return false;
	}

	size_t size = strlen(text);
	size_t r_size = fwrite(text, 1, size, file);

	if (r_size != size) {
		d_fail("failed to write: '%s'\n", path);
		return false;
	}

	fclose(file);

	return true;

}

static bool write_bytes(const char *path, const uint8_t *data, size_t size) {

	FILE *file = fopen(path, "wb");

	if (!file) {
		d_fail("failed to write: '%s'\n", path);
		return false;
	}

	size_t r_size = fwrite(data, 1, size, file);

	if (r_size != size) {
		d_fail("failed to write: '%s'\n", path);
		return false;
	}

	fclose(file);

	return true;

}

static bool is_file(const char *path) {
	struct stat sb;
	return stat(path, &sb) == 0 && S_ISREG(sb.st_mode);
}

static bool is_dir(const char *path) {
	struct stat sb;
	return stat(path, &sb) == 0 && S_ISDIR(sb.st_mode);
}

static char **glob(const char *path, const char *ext) {

	DIR *dir = opendir(path);

	if (!dir) {
		d_fail("failed to read: '%s'\n", path);
		return NULL;
	}

	struct dirent *entry;
	int num = 0;

	while ((entry = readdir(dir))) {
		char *aext = d_extname(entry->d_name);
		if (strcmp(ext, aext) == 0) {
			num++;
		}
		free(aext);
	}

	rewinddir(dir);
	char **list = malloc(sizeof(char*) * (num + 1));
	num = 0;

	while ((entry = readdir(dir))) {
		char *aext = d_extname(entry->d_name);
		if (strcmp(ext, aext) == 0) {
			list[num] = malloc(strlen(entry->d_name) + 1);
			strcpy(list[num], entry->d_name);
			num++;
		}
		free(aext);
	}

	list[num] = NULL;
	closedir(dir);

	return list;

}

void d_free_dir(char **list) {
	for (int i = 0; list[i] != NULL; i++) {
		free(list[i]);
	}
	free(list);
}

char *d_read_text(const char *path) {
	return read_text(d_fmt("%s%s", d_fs.res_path, path));
}

uint8_t *d_read_bytes(const char *path, size_t *size) {
	return read_bytes(d_fmt("%s%s", d_fs.res_path, path), size);
}

char **d_glob(const char *path, const char *ext) {
	return glob(d_fmt("%s%s", d_fs.res_path, path), ext);
}

bool d_is_file(const char *path) {
	return is_file(d_fmt("%s%s", d_fs.res_path, path));
}

bool d_is_dir(const char *path) {
	return is_dir(d_fmt("%s%s", d_fs.res_path, path));
}

char *d_data_read_text(const char *path) {
	return read_text(d_fmt("%s%s", d_fs.data_path, path));
}

uint8_t *d_data_read_bytes(const char *path, size_t *size) {
	return read_bytes(d_fmt("%s%s", d_fs.data_path, path), size);
}

bool d_data_is_file(const char *path) {
	return is_file(d_fmt("%s%s", d_fs.data_path, path));
}

bool d_data_is_dir(const char *path) {
	return is_dir(d_fmt("%s%s", d_fs.data_path, path));
}

// TODO: recursive mkdir?
void d_data_write_text(const char *path, const char *content) {
	write_text(d_fmt("%s%s", d_fs.data_path, path), content);
}

void d_data_write_bytes(const char *path, const uint8_t *content, size_t size) {
	write_bytes(d_fmt("%s%s", d_fs.data_path, path), content, size);
}

char *d_extname(const char *path) {
	const char *dot = strrchr(path, '.');
	if (!dot) {
		return NULL;
	}
	char *buf = malloc(strlen(dot + 1) + 1);
	strcpy(buf, dot + 1);
	return buf;
}

char *d_basename(const char *path) {
	const char *dot = strrchr(path, '.');
	const char *slash = strrchr(path, '/');
	dot = dot ? dot : path + strlen(path);
	slash = slash ? slash + 1 : path;
	int len = dot - slash;
	char *buf = malloc(len + 1);
	strncpy(buf, slash, len);
	buf[len] = '\0';
	return buf;
}

// math                   ~                       o                   ~
//         ~         .                 ~          .          O
//   .         _              O                              o
//        .  ><_'>            o           .         .        .
//                            .               .                    .
//  .                   ~          .                  ~
//              o           ~               ~                   .
//      O       .               .        _                  O
//      o                .             <'_><        .       o         .
//      .    .                                              .
//                ~         .     o             ~                   .
//   ~               .            .          .                 ~

d_rng d_make_rng(uint64_t seed) {
	return (d_rng) {
		.seed = seed,
	};
}

float d_rng_gen(d_rng *rng) {
	rng->seed = (D_RNG_A * rng->seed + D_RNG_C) % D_RNG_M;
	return (float)(rng->seed) / (float)D_RNG_M;
}

float randf(float low, float hi) {
	return low + (float)rand() / (float)RAND_MAX * (hi - low);
}

vec2 vec2f(float x, float y) {
	return (vec2) {
		.x = x,
		.y = y,
	};
}

vec2 vec2u() {
	return vec2f(0.0, 0.0);
}

vec2 vec2_add(vec2 p1, vec2 p2) {
	return (vec2) {
		.x = p1.x + p2.x,
		.y = p1.y + p2.y,
	};
}

vec2 vec2_sub(vec2 p1, vec2 p2) {
	return (vec2) {
		.x = p1.x - p2.x,
		.y = p1.y - p2.y,
	};
}

vec2 vec2_scale(vec2 p, float s) {
	return (vec2) {
		.x = p.x * s,
		.y = p.y * s,
	};
}

vec2 vec2_mult(vec2 p1, vec2 p2) {
	return (vec2) {
		.x = p1.x * p2.x,
		.y = p1.y * p2.y,
	};
}

float vec2_dist(vec2 p1, vec2 p2) {
	return sqrt(
		(p1.x - p2.x) * (p1.x - p2.x) +
		(p1.y - p2.y) * (p1.y - p2.y)
	);
}

float vec2_len(vec2 p) {
	return vec2_dist(p, vec2f(0.0, 0.0));
}

vec2 vec2_unit(vec2 p) {
	return vec2_scale(p, 1.0 / vec2_len(p));
}

vec3 vec2_cross(vec2 p1, vec2 p2) {
	return vec3_cross(vec3f(p1.x, p1.y, 0.0), vec3f(p2.x, p2.y, 0.0));
}

float vec2_dot(vec2 p1, vec2 p2) {
	return p1.x * p2.x + p1.y * p2.y;
}

vec2 vec2_normal(vec2 p) {
	return vec2f(p.y, -p.x);
}

float vec2_angle(vec2 p1, vec2 p2) {
	return atan2(p1.y - p2.y, p1.x - p2.x);
}

vec2 vec2_min(vec2 a, vec2 b) {
	return (vec2) {
		.x = fminf(a.x, b.x),
		.y = fminf(a.y, b.y),
	};
}

vec2 vec2_max(vec2 a, vec2 b) {
	return (vec2) {
		.x = fmaxf(a.x, b.x),
		.y = fmaxf(a.y, b.y),
	};
}

vec2 vec2_lerp(vec2 from, vec2 to, float t) {
	return (vec2) {
		.x = lerpf(from.x, to.x, t),
		.y = lerpf(from.y, to.y, t),
	};
}

vec2 vec2_rand(vec2 p1, vec2 p2) {
	return (vec2) {
		.x = randf(p1.x, p2.x),
		.y = randf(p1.y, p2.y),
	};
}

vec2 vec2_clamp(vec2 p, vec2 low, vec2 hi) {
	return (vec2) {
		.x = clampf(p.x, low.x, hi.x),
		.y = clampf(p.x, low.y, hi.y),
	};
}

bool vec2_eq(vec2 p1, vec2 p2) {
	return p1.x == p2.x && p1.y == p2.y;
}

const char *vec2_fmt(vec2 p) {
	return d_fmt("vec2(%.8g, %.8g)", p.x, p.y);
}

vec3 vec3f(float x, float y, float z) {
	return (vec3) {
		.x = x,
		.y = y,
		.z = z,
	};
}

vec3 vec3u() {
	return vec3f(0.0, 0.0, 0.0);
}

vec3 vec3_add(vec3 p1, vec3 p2) {
	return (vec3) {
		.x = p1.x + p2.x,
		.y = p1.y + p2.y,
		.z = p1.z + p2.z,
	};
}

vec3 vec3_sub(vec3 p1, vec3 p2) {
	return (vec3) {
		.x = p1.x - p2.x,
		.y = p1.y - p2.y,
		.z = p1.z - p2.z,
	};
}

vec3 vec3_scale(vec3 p, float s) {
	return (vec3) {
		.x = p.x * s,
		.y = p.y * s,
		.z = p.z * s,
	};
}

vec3 vec3_mult(vec3 p1, vec3 p2) {
	return (vec3) {
		.x = p1.x * p2.x,
		.y = p1.y * p2.y,
		.z = p1.z * p2.z,
	};
}

float vec3_dist(vec3 p1, vec3 p2) {
	return sqrt(
		(p1.x - p2.x) * (p1.x - p2.x) +
		(p1.y - p2.y) * (p1.y - p2.y) +
		(p1.z - p2.z) * (p1.z - p2.z)
	);
}

float vec3_len(vec3 p) {
	return vec3_dist(p, vec3f(0.0, 0.0, 0.0));
}

vec3 vec3_unit(vec3 p) {
	return vec3_scale(p, 1.0 / vec3_len(p));
}

vec3 vec3_cross(vec3 p1, vec3 p2) {
	return (vec3) {
		.x = (p1.y * p2.z) - (p1.z * p2.y),
		.y = (p1.z * p2.x) - (p1.x * p2.z),
		.z = (p1.x * p2.y) - (p1.y * p2.x),
	};
}

float vec3_dot(vec3 p1, vec3 p2) {
	return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}

vec3 vec3_min(vec3 a, vec3 b) {
	return (vec3) {
		.x = fminf(a.x, b.x),
		.y = fminf(a.y, b.y),
		.z = fminf(a.z, b.z),
	};
}

vec3 vec3_max(vec3 a, vec3 b) {
	return (vec3) {
		.x = fmaxf(a.x, b.x),
		.y = fmaxf(a.y, b.y),
		.z = fmaxf(a.z, b.z),
	};
}

vec3 vec3_lerp(vec3 from, vec3 to, float t) {
	return (vec3) {
		.x = lerpf(from.x, to.x, t),
		.y = lerpf(from.y, to.y, t),
		.z = lerpf(from.z, to.z, t),
	};
}

vec3 vec3_rand(vec3 p1, vec3 p2) {
	return (vec3) {
		.x = randf(p1.x, p2.x),
		.y = randf(p1.y, p2.y),
		.z = randf(p1.z, p2.z),
	};
}

vec3 vec3_clamp(vec3 p, vec3 low, vec3 hi) {
	return (vec3) {
		.x = clampf(p.x, low.x, hi.x),
		.y = clampf(p.x, low.y, hi.y),
		.z = clampf(p.x, low.z, hi.z),
	};
}

bool vec3_eq(vec3 p1, vec3 p2) {
	return p1.x == p2.x && p1.y == p2.y && p1.z == p2.z;
}

const char *vec3_fmt(vec3 p) {
	return d_fmt("vec3(%.8g, %.8g, %.8g)", p.x, p.y, p.z);
}

vec4 vec4f(float x, float y, float z, float w) {
	return (vec4) {
		.x = x,
		.y = y,
		.z = z,
		.w = w,
	};
}

vec4 vec4u() {
	return vec4f(0.0, 0.0, 0.0, 1.0);
}

color colori(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	return (color) {
		.r = r,
		.g = g,
		.b = b,
		.a = a,
	};
}

color colorf(float r, float g, float b, float a) {
	return (color) {
		.r = r * 255,
		.g = g * 255,
		.b = b * 255,
		.a = a * 255,
	};
}

color colorx(uint32_t hex) {
	return (color) {
		.r = ((hex >> 24) & 0xff),
		.g = ((hex >> 16) & 0xff),
		.b = ((hex >> 8) & 0xff),
		.a = ((hex >> 0) & 0xff),
	};
}

color coloru() {
	return (color) {
		.r = 255,
		.g = 255,
		.b = 255,
		.a = 255,
	};
}

color color_mix(color c1, color c2) {
	return (color) {
		.r = c1.r * c2.r / 255,
		.g = c1.g * c2.g / 255,
		.b = c1.b * c2.b / 255,
		.a = c1.a * c2.a / 255,
	};
}

bool color_eq(color c1, color c2) {
	return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b && c1.a == c2.a;
}

const char *color_fmt(color c) {
	return d_fmt("color(%d, %d, %d, %d)", c.r, c.g, c.b, c.a);
}

mat4 mat4f(
	float m0, float m1, float m2, float m3,
	float m4, float m5, float m6, float m7,
	float m8, float m9, float m10, float m11,
	float m12, float m13, float m14, float m15
) {
	return (mat4) {
		.m = {
			m0, m1, m2, m3,
			m4, m5, m6, m7,
			m8, m9, m10, m11,
			m12, m13, m14, m15
		},
	};
}

mat4 mat4u() {
	return mat4_identity();
}

mat4 mat4_identity() {
	return (mat4) {
		.m = {
			1.0, 0.0, 0.0, 0.0,
			0.0, 1.0, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			0.0, 0.0, 0.0, 1.0,
		},
	};
}

mat4 mat4_mult(mat4 m1, mat4 m2) {

	mat4 out = mat4u();

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			out.m[i * 4 + j] =
				m1.m[0 * 4 + j] * m2.m[i * 4 + 0] +
				m1.m[1 * 4 + j] * m2.m[i * 4 + 1] +
				m1.m[2 * 4 + j] * m2.m[i * 4 + 2] +
				m1.m[3 * 4 + j] * m2.m[i * 4 + 3];
		}
	}

	return out;

}

vec4 mat4_mult_vec4(mat4 m, vec4 p) {
	return (vec4) {
		.x = p.x * m.m[0] + p.y * m.m[4] + p.z * m.m[8] + p.w * m.m[12],
		.y = p.x * m.m[1] + p.y * m.m[5] + p.z * m.m[9] + p.w * m.m[13],
		.z = p.x * m.m[2] + p.y * m.m[6] + p.z * m.m[10] + p.w * m.m[14],
		.w = p.x * m.m[3] + p.y * m.m[7] + p.z * m.m[11] + p.w * m.m[15]
	};
}

vec3 mat4_mult_vec3(mat4 m, vec3 p) {
	vec4 p4 = mat4_mult_vec4(m, vec4f(p.x, p.y, p.z, 1.0));
	return vec3f(p4.x, p4.y, p4.z);
}

vec2 mat4_mult_vec2(mat4 m, vec2 p) {
	vec3 p3 = mat4_mult_vec3(m, vec3f(p.x, p.y, 0.0));
	return vec2f(p3.x, p3.y);
}

mat4 mat4_invert(mat4 m) {

	mat4 out = mat4u();

	float f00 = m.m[10] * m.m[15] - m.m[14] * m.m[11];
	float f01 = m.m[9] * m.m[15] - m.m[13] * m.m[11];
	float f02 = m.m[9] * m.m[14] - m.m[13] * m.m[10];
	float f03 = m.m[8] * m.m[15] - m.m[12] * m.m[11];
	float f04 = m.m[8] * m.m[14] - m.m[12] * m.m[10];
	float f05 = m.m[8] * m.m[13] - m.m[12] * m.m[9];
	float f06 = m.m[6] * m.m[15] - m.m[14] * m.m[7];
	float f07 = m.m[5] * m.m[15] - m.m[13] * m.m[7];
	float f08 = m.m[5] * m.m[14] - m.m[13] * m.m[6];
	float f09 = m.m[4] * m.m[15] - m.m[12] * m.m[7];
	float f10 = m.m[4] * m.m[14] - m.m[12] * m.m[6];
	float f11 = m.m[5] * m.m[15] - m.m[13] * m.m[7];
	float f12 = m.m[4] * m.m[13] - m.m[12] * m.m[5];
	float f13 = m.m[6] * m.m[11] - m.m[10] * m.m[7];
	float f14 = m.m[5] * m.m[11] - m.m[9] * m.m[7];
	float f15 = m.m[5] * m.m[10] - m.m[9] * m.m[6];
	float f16 = m.m[4] * m.m[11] - m.m[8] * m.m[7];
	float f17 = m.m[4] * m.m[10] - m.m[8] * m.m[6];
	float f18 = m.m[4] * m.m[9] - m.m[8] * m.m[5];

	out.m[0] = m.m[5] * f00 - m.m[6] * f01 + m.m[7] * f02;
	out.m[4] = -(m.m[4] * f00 - m.m[6] * f03 + m.m[7] * f04);
	out.m[8] = m.m[4] * f01 - m.m[5] * f03 + m.m[7] * f05;
	out.m[12] = -(m.m[4] * f02 - m.m[5] * f04 + m.m[6] * f05);

	out.m[1] = -(m.m[1] * f00 - m.m[2] * f01 + m.m[3] * f02);
	out.m[5] = m.m[0] * f00 - m.m[2] * f03 + m.m[3] * f04;
	out.m[9] = -(m.m[0] * f01 - m.m[1] * f03 + m.m[3] * f05);
	out.m[13] = m.m[0] * f02 - m.m[1] * f04 + m.m[2] * f05;

	out.m[2] = m.m[1] * f06 - m.m[2] * f07 + m.m[3] * f08;
	out.m[6] = -(m.m[0] * f06 - m.m[2] * f09 + m.m[3] * f10);
	out.m[10] = m.m[0] * f11 - m.m[1] * f09 + m.m[3] * f12;
	out.m[14] = -(m.m[0] * f08 - m.m[1] * f10 + m.m[2] * f12);

	out.m[3] = -(m.m[1] * f13 - m.m[2] * f14 + m.m[3] * f15);
	out.m[7] = m.m[0] * f13 - m.m[2] * f16 + m.m[3] * f17;
	out.m[11] = -(m.m[0] * f14 - m.m[1] * f16 + m.m[3] * f18);
	out.m[15] = m.m[0] * f15 - m.m[1] * f17 + m.m[2] * f18;

	float det =
		m.m[0] * out.m[0] +
		m.m[1] * out.m[4] +
		m.m[2] * out.m[8] +
		m.m[3] * out.m[12];

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			out.m[i * 4 + j] *= (1.0 / det);
		}
	}

	return out;

}

mat4 mat4_scale(vec3 s) {
	return (mat4) {
		.m = {
			s.x, 0.0, 0.0, 0.0,
			0.0, s.y, 0.0, 0.0,
			0.0, 0.0, s.z, 0.0,
			0.0, 0.0, 0.0, 1.0,
		},
	};
}

mat4 mat4_translate(vec3 p) {
	return (mat4) {
		.m = {
			1.0, 0.0, 0.0, 0.0,
			0.0, 1.0, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			p.x, p.y, p.z, 1.0,
		},
	};
}

mat4 mat4_rot_x(float a) {
	return (mat4) {
		.m = {
			1.0, 0.0, 0.0, 0.0,
			0.0, cos(a), -sin(a), 0.0,
			0.0, sin(a), cos(a), 0.0,
			0.0, 0.0, 0.0, 1.0,
		},
	};
}

mat4 mat4_rot_y(float a) {
	return (mat4) {
		.m = {
			cos(a), 0.0, -sin(a), 0.0,
			0.0, 1.0, 0.0, 0.0,
			sin(a), 0.0, cos(a), 0.0,
			0.0, 0.0, 0.0, 1.0,
		},
	};
}

mat4 mat4_rot_z(float a) {
	return (mat4) {
		.m = {
			cos(a), -sin(a), 0.0, 0.0,
			sin(a), cos(a), 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			0.0, 0.0, 0.0, 1.0,
		},
	};
}

mat4 mat4_rot_quat(quat q) {
	return mat4_mult(
		mat4f(
			q.w, q.z, -q.y, q.x,
			-q.z, q.w, q.x, q.y,
			q.y, -q.x, q.w, q.z,
			-q.x, -q.y, -q.z, q.w
		),
		mat4f(
			q.w, q.z, -q.y, -q.x,
			-q.z, q.w, q.x, -q.y,
			q.y, -q.x, q.w, -q.z,
			q.x, q.y, q.z, q.w
		)
	);
}

mat4 mat4_ortho(float w, float h, float near, float far) {

	float left = -w / 2.0;
	float right = w / 2.0;
	float bottom = -h / 2.0;
	float top = h / 2.0;
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	float tz = -(far + near) / (far - near);

	return (mat4) {
		.m = {
			2.0 / (right - left), 0.0, 0.0, 0.0,
			0.0, 2.0 / (top - bottom), 0.0, 0.0,
			0.0, 0.0, -2.0 / (far - near), 0.0,
			tx, ty, tz, 1.0,
		},
	};

}

mat4 mat4_persp(float fov, float aspect, float near, float far) {

	float f = 1.0 / tan(fov / 2.0);

	return (mat4) {
		.m = {
			-f / aspect, 0.0, 0.0, 0.0,
			0.0, f, 0.0, 0.0,
			0.0, 0.0, (far + near) / (far - near), 1.0,
			0.0, 0.0, -(2.0 * far * near) / (far - near), 0.0,
		}
	};

}

mat4 mat4_view(vec3 pos, vec3 dir, vec3 up) {

	vec3 z = vec3_unit(dir);
	vec3 x = vec3_unit(vec3_cross(up, z));
	vec3 y = vec3_cross(z, x);

	return (mat4) {
		.m = {
			x.x, y.x, z.x, 0.0,
			x.y, y.y, z.y, 0.0,
			x.z, y.z, z.z, 0.0,
			-vec3_dot(x, pos), -vec3_dot(y, pos), -vec3_dot(z, pos), 1.0,
		},
	};

}

const char *mat4_fmt(mat4 m) {
	return d_fmt(
		"mat4(\n"
		"     %.8g, %.8g, %.8g, %.8g,\n"
		"     %.8g, %.8g, %.8g, %.8g,\n"
		"     %.8g, %.8g, %.8g, %.8g,\n"
		"     %.8g, %.8g, %.8g, %.8g,\n",
		")",
		m.m[0], m.m[1], m.m[2], m.m[3],
		m.m[4], m.m[5], m.m[6], m.m[7],
		m.m[8], m.m[9], m.m[10], m.m[11],
		m.m[12], m.m[13], m.m[14], m.m[15]
	);
}

quat quatf(float x, float y, float z, float w) {
	return (quat) {
		.x = x,
		.y = y,
		.z = z,
		.w = w,
	};
}

quat quatu() {
	return quatf(0.0, 0.0, 0.0, 1.0);
}

quad quadf(float x, float y, float w, float h) {
	return (quad) {
		.x = x,
		.y = y,
		.w = w,
		.h = h,
	};
}

quad quadu() {
	return quadf(0.0, 0.0, 1.0, 1.0);
}

const char *quad_fmt(quad q) {
	return d_fmt("quad(%.8g, %.8g, %.8g, %.8g)", q.x, q.y, q.w, q.h);
}

rect rectf(vec2 p1, vec2 p2) {
	return (rect) {
		.p1 = p1,
		.p2 = p2,
	};
}

box boxf(vec3 p1, vec3 p2) {
	return (box) {
		.p1 = p1,
		.p2 = p2,
	};
}

line2 line2f(vec2 p1, vec2 p2) {
	return (line2) {
		.p1 = p1,
		.p2 = p2,
	};
}

line3 line3f(vec3 p1, vec3 p2) {
	return (line3) {
		.p1 = p1,
		.p2 = p2,
	};
}

circle circlef(vec2 center, float radius) {
	return (circle) {
		.center = center,
		.radius = radius,
	};
}

sphere spheref(vec3 center, float radius) {
	return (sphere) {
		.center = center,
		.radius = radius,
	};
}

plane planef(vec3 normal, float dist) {
	return (plane) {
		.normal = normal,
		.dist = dist,
	};
}

ray2 ray2f(vec2 origin, vec2 dir) {
	return (ray2) {
		.origin = origin,
		.dir = dir,
	};
}

ray3 ray3f(vec3 origin, vec3 dir) {
	return (ray3) {
		.origin = origin,
		.dir = dir,
	};
}

float degf(float r) {
	return r * (180.0 / D_PI);
}

float radf(float d) {
	return d / (180.0 / D_PI);
}

int maxi(int a, int b) {
	return a > b ? a : b;
}

int mini(int a, int b) {
	return a < b ? a : b;
}

float clampf(float v, float low, float hi) {
	return fmaxf(low, fminf(v, hi));
}

int clampi(int v, int low, int hi) {
	return maxi(low, mini(v, hi));
}

float lerpf(float a, float b, float t) {
	return a + (b - a) * t;
}

float mapf(float v, float l1, float h1, float l2, float h2) {
	return l2 + (v - l1) / (h1 - l1) * (h2 - l2);
}

static void fix_rect(rect *r) {
	vec2 pp1 = vec2_min(r->p1, r->p2);
	vec2 pp2 = vec2_max(r->p1, r->p2);
	r->p1 = pp1;
	r->p2 = pp2;
}

bool pt_rect(vec2 pt, rect r) {
	fix_rect(&r);
	return
		pt.x >= r.p1.x
		&& pt.x <= r.p2.x
		&& pt.y >= r.p1.y
		&& pt.y <= r.p2.y;
}

bool rect_rect(rect r1, rect r2) {
	fix_rect(&r1);
	fix_rect(&r2);
	return
		r1.p2.x >= r2.p1.x
		&& r1.p1.x <= r2.p2.x
		&& r1.p2.y >= r2.p1.y
		&& r1.p1.y <= r2.p2.y;
}

bool line_line(line2 l1, line2 l2) {
	float a =
		(
			(l2.p2.x - l2.p1.x)
			* (l1.p1.y - l2.p1.y)
			- (l2.p2.y - l2.p1.y)
			* (l1.p1.x - l2.p1.x)
		)
		/
		(
			(l2.p2.y - l2.p1.y)
			* (l1.p2.x - l1.p1.x)
			- (l2.p2.x - l2.p1.x)
			* (l1.p2.y - l1.p1.y)
		);
	float b =
		(
			(l1.p2.x - l1.p1.x)
			* (l1.p1.y - l2.p1.y)
			- (l1.p2.y - l1.p1.y)
			* (l1.p1.x - l2.p1.x)
		)
		/
		(
			(l2.p2.y - l2.p1.y)
			* (l1.p2.x - l1.p1.x)
			- (l2.p2.x - l2.p1.x)
			* (l1.p2.y - l1.p1.y)
		);
	return a >= 0.0 && a <= 1.0 && b >= 0.0 && b <= 1.0;
}

bool line_rect(line2 l, rect r) {
	if (pt_rect(l.p1, r) || pt_rect(l.p2, r)) {
		return true;
	}
	return
		line_line(l, line2f(r.p1, vec2f(r.p2.x, r.p1.y)))
		|| line_line(l, line2f(vec2f(r.p2.x, r.p1.y), r.p2))
		|| line_line(l, line2f(r.p2, vec2f(r.p1.x, r.p2.y)))
		|| line_line(l, line2f(vec2f(r.p1.x, r.p2.y), r.p1));
}

bool pt_circle(vec2 pt, circle c) {
	return vec2_dist(pt, c.center) <= c.radius;
}

bool circle_circle(circle c1, circle c2) {
	return vec2_dist(c1.center, c2.center) <= c1.radius + c2.radius;
}

// utils                  ~                       o                   ~
//         ~         .                 ~          .          O
//   .         _              O                              o
//        .  ><_'>            o           .         .        .
//                            .               .                    .
//  .                   ~          .                  ~
//              o           ~               ~                   .
//      O       .               .        _                  O
//      o                .             <'_><        .       o         .
//      .    .                                              .
//                ~         .     o             ~                   .
//   ~               .            .          .                 ~

#define FMT_BUFSIZE 256

const char *d_fmt(const char *fmt, ...) {

	static char buf[FMT_BUFSIZE];
	va_list args;

	va_start(args, fmt);
	vsnprintf(buf, FMT_BUFSIZE, fmt, args);
	va_end(args);

	return buf;

}

char *d_fmta(const char *fmt, ...) {

	va_list args;

	va_start(args, fmt);
	int size = vsnprintf(NULL, 0, fmt, args) + 1;
	va_end(args);

	char *buf = malloc(size);

	va_start(args, fmt);
	vsnprintf(buf, size, fmt, args);
	va_end(args);

	return buf;

}

#endif // DIRTY_IMPL_ONCE
#endif // DIRTY_IMPL

//                        ~                       o                   ~
//         ~         .                 ~          .          O
//   .         _              O                              o
//        .  ><_'>            o           .         .        .
//                            .               .                    .
//  .                   ~          .                  ~          (
//              o           ~               ~                   . )
//      O       .      (       .        _                  O     (
//      o               )             <'_><        .       o      )  .
//      .  ( .         (                        )          .     (
//          )      ~    )     .     o          (  ~               )    .
//         (       .   (    )   .          .    )            ~   (
//  _.   _  )_   .  __  )_  (  _  .   _  __   _( __    _     ._  _)   _

