// TODO: gamepad
// TODO: scale mode

#ifndef D_APP_H
#define D_APP_H

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

#ifndef D_MATH_H
#include "d_math.h"
#endif

#include <stdbool.h>

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
	D_KEY_BACKTICK,
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
	D_KEY_DELETE,
	D_KEY_ENTER,
	D_KEY_INSERT,
	D_KEY_MENU,
	D_KEY_HOME,
	D_KEY_PAGE_UP,
	D_KEY_PAGE_DOWN,
	D_KEY_LBRACKET,
	D_KEY_RBRACKET,
	D_KEY_LPAREN,
	D_KEY_RPAREN,
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
	D_KEYMOD_ALT,
	D_KEYMOD_META,
	D_KEYMOD_CTRL,
	D_KEYMOD_SHIFT,
} d_keymod;

typedef enum {
	D_MOUSE_NONE,
	D_MOUSE_LEFT,
	D_MOUSE_RIGHT,
	D_MOUSE_MIDDLE,
	_D_NUM_MOUSE,
} d_mouse;

typedef enum {
	D_SCALEMODE_STRETCH,
	D_SCALEMODE_LETTERBOX,
	D_SCALEMODE_FIT,
} d_scale_mode;

typedef struct {
	void (*init)(void);
	void (*frame)(void);
	void (*quit)(void);
	char* title;
	int width;
	int height;
	bool fullscreen;
	bool vsync;
	bool hidpi;
	char* canvas_root;
} d_app_desc;

typedef uint8_t d_touch;

void d_app_run(d_app_desc desc);
void d_app_quit(void);
void d_app_present(int w, int h, d_color* buf);

bool d_app_is_fullscreen(void);
void d_app_set_fullscreen(bool b);

bool d_app_is_mouse_locked(void);
void d_app_set_mouse_locked(bool b);

bool d_app_is_mouse_hidden(void);
void d_app_set_mouse_hidden(bool b);

void d_app_set_title(char* title);

bool d_app_is_keyboard_shown(void);
void d_app_set_keyboard_shown(bool b);

int d_app_width(void);
int d_app_height(void);

float d_app_time(void);
float d_app_dt(void);
int d_app_fps(void);

bool d_app_key_pressed(d_key k);
bool d_app_key_rpressed(d_key k);
bool d_app_key_released(d_key k);
bool d_app_mouse_pressed(d_mouse m);
bool d_app_mouse_released(d_mouse m);
bool d_app_mouse_moved(void);
bool d_app_touch_pressed(d_touch t);
bool d_app_touch_released(d_touch t);
bool d_app_touch_moved(d_touch t);
bool d_app_scrolled(void);
d_vec2 d_app_wheel(void);
bool d_app_resized(void);
char d_app_input(void);
bool d_app_has_focus(void);

bool d_app_mouse_down(d_mouse m);
bool d_app_key_down(d_key k);
bool d_app_key_mod(d_keymod kmod);
d_vec2 d_app_mouse_pos(void);
d_vec2 d_app_mouse_dpos(void);
d_vec2 d_app_touch_pos(d_touch t);
d_vec2 d_app_touch_dpos(d_touch t);

#endif

#ifdef D_IMPL
#define D_APP_IMPL
#endif

#ifdef D_APP_IMPL
#ifndef D_APP_IMPL_ONCE
#define D_APP_IMPL_ONCE

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <sys/time.h>

#if !defined(D_CPU) && !defined(D_GL) && !defined(D_METAL) && !defined(D_TERM)
	#define D_CPU
#endif

#if defined(D_METAL) && !defined(D_MACOS) && !defined(D_IOS)
	#error "D_METAL is only for macOS or iOS"
#endif

#if \
	!defined(D_COCOA) && \
	!defined(D_UIKIT) && \
	!defined(D_WIN32) && \
	!defined(D_X11) && \
	!defined(D_CANVAS) && \
	!defined(D_TERM)
	#if defined(D_MACOS)
		#define D_COCOA
	#elif defined(D_IOS)
		#define D_UIKIT
	#elif defined(D_WINDOWS)
		#define D_WIN32
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
#elif defined(D_WIN32)
	#include <windows.h>
    #include <windowsx.h>
#elif defined(D_X11)
	#include <X11/Xlib.h>
	#if defined(D_GL)
		#include <GL/glx.h>
	#endif
#elif defined(D_WAYLAND)
	#include <wayland-client.h>
#endif

#define D_MAX_TOUCHES 8
#define D_DEF_WIDTH 256
#define D_DEF_HEIGHT 256
#define D_DEF_SCALEMODE D_SCALEMODE_STRETCH

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
	D_BTN_DOWN,
	D_BTN_RELEASED,
} d_btn_state;

typedef struct {
	uintptr_t id;
	d_vec2 pos;
	d_vec2 dpos;
	d_btn_state state;
} d_touch_state;

typedef struct {

	d_app_desc desc;
	struct timeval start_time;
	float time;
	float dt;
	int width;
	int height;
	d_vec2 mouse_pos;
	d_vec2 mouse_dpos;
	bool touching;
	d_vec2 wheel;
	d_btn_state key_states[_D_NUM_KEYS];
	d_btn_state mouse_states[_D_NUM_MOUSE];
	d_touch_state touches[D_MAX_TOUCHES];
	int num_touches;
	bool has_focus;
	bool resized;
	char char_input;
	float fps_timer;
	int fps;
	bool quit;
	d_scale_mode scale_mode;

#if defined(D_COCOA)
	NSWindow* window;
	DView* view;
#elif defined(D_UIKIT)
	UIWindow* window;
	DView* view;
#elif defined(D_WIN32)
	HWND window;
#elif defined(D_X11)
	Display* display;
	Visual* visual;
	unsigned int depth;
	GC gc;
	Window window;
#endif

#if defined(D_METAL)
	id<MTLDevice> mtl_dev;
	id<MTLCommandQueue> mtl_queue;
	id<MTLRenderPipelineState> mtl_pip;
#endif

#if defined(D_GL)
	GLuint gl_prog;
	GLuint gl_vbuf;
#endif

} d_app_ctx;

static d_app_ctx d_app;

static void process_btn(d_btn_state* b) {
	if (*b == D_BTN_PRESSED || *b == D_BTN_RPRESSED) {
		*b = D_BTN_DOWN;
	} else if (*b == D_BTN_RELEASED) {
		*b = D_BTN_IDLE;
	}
}

static void d_app_init(void) {
	if (d_app.desc.init) {
		d_app.desc.init();
	}
}

static void d_app_frame(void) {

	if (d_app.desc.frame) {
		d_app.desc.frame();
	}

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
		process_btn(&d_app.key_states[i]);
	}

	for (int i = 0; i < _D_NUM_MOUSE; i++) {
		process_btn(&d_app.mouse_states[i]);
	}

	for (int i = 0; i < d_app.num_touches; i++) {
		process_btn(&d_app.touches[i].state);
		if (d_app.touches[i].state == D_BTN_IDLE) {
			d_app.touches[i--] = d_app.touches[--d_app.num_touches];
			continue;
		}
	}

	d_app.wheel.x = 0.0;
	d_app.wheel.y = 0.0;
	d_app.resized = false;
	d_app.mouse_dpos = d_vec2f(0.0, 0.0);
	d_app.char_input = 0;

}

// -------------------------------------------------------------
// OpenGL
#if defined(D_GL)

static float quad_verts[] = {
	-1, -1, 0, 1,
	-1, 1, 0, 0,
	1, -1, 1, 1,
	1, 1, 1, 0,
};

static char const* vs_src =
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

static const char* fs_src =
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

static void d_gl_init(void) {

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
		fprintf(stderr, "%s", info_log);
	}

	// fragment shader
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fs, 1, &fs_src, 0);
	glCompileShader(fs);

	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE) {
		glGetShaderInfoLog(fs, 512, NULL, info_log);
		fprintf(stderr, "%s", info_log);
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
		fprintf(stderr, "%s", info_log);
	}

	// vbuf
	glGenBuffers(1, &d_app.gl_vbuf);
	glBindBuffer(GL_ARRAY_BUFFER, d_app.gl_vbuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_verts), quad_verts, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// init gl
	glViewport(0, 0, d_app_width(), d_app_height());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

}

static void d_gl_present(int w, int h, d_color* buf) {

	glBindBuffer(GL_ARRAY_BUFFER, d_app.gl_vbuf);
	glUseProgram(d_app.gl_prog);

	GLuint tex;

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		w,
		h,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		buf
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

static float quad_verts[] = {
	-1, -1, 0, 1,
	-1, 1, 0, 0,
	1, -1, 1, 1,
	1, 1, 1, 0,
};

static char* shader_src =
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
	"const device VertexIn* verts [[ buffer(0) ]],"
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

static void d_mtl_init(void) {

	id<MTLDevice> dev = MTLCreateSystemDefaultDevice();
	id<MTLCommandQueue> queue = [dev newCommandQueue];

	NSError* err = NULL;

	id<MTLLibrary> lib = [dev
		newLibraryWithSource:[NSString stringWithUTF8String:shader_src]
		options:nil
		error:&err
	];

	if (err) {
		fprintf(stderr, "%s\n", [err.localizedDescription UTF8String]);
	}

	MTLRenderPipelineDescriptor* pip_desc = [[MTLRenderPipelineDescriptor alloc] init];
	pip_desc.vertexFunction = [lib newFunctionWithName:@"vert"];
	pip_desc.fragmentFunction = [lib newFunctionWithName:@"frag"];
	pip_desc.colorAttachments[0].pixelFormat = MTLPixelFormatRGBA8Unorm;

	id<MTLRenderPipelineState> pip = [dev
		newRenderPipelineStateWithDescriptor:pip_desc
		error:&err
	];

	if (err) {
		fprintf(stderr, "%s\n", [err.localizedDescription UTF8String]);
	}

	d_app.mtl_dev = dev;
	d_app.mtl_queue = queue;
	d_app.mtl_pip = pip;
	d_app.view.device = dev;

}

static void d_mtl_present(int w, int h, d_color* buf) {

	id<MTLCommandBuffer> cmd_buf = [d_app.mtl_queue commandBuffer];
	MTLRenderPassDescriptor* desc = [d_app.view currentRenderPassDescriptor];

	MTLTextureDescriptor* tex_desc = [MTLTextureDescriptor
		texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
		width:w
		height:h
		mipmapped:false
	];

	id<MTLTexture> tex = [d_app.mtl_dev newTextureWithDescriptor:tex_desc];

	MTLRegion region = {
		{ 0, 0, 0 },
		{ w, h, 1 }
	};

	[tex
		replaceRegion:region
		mipmapLevel:0
		withBytes:buf
		bytesPerRow:w * 4
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
		setFragmentTexture:tex
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
		case 0x32: return D_KEY_BACKTICK;
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

static void d_cocoa_present(int w, int h, d_color* buf) {

	CGContextRef ctx = [[NSGraphicsContext currentContext] CGContext];
	CGContextSetInterpolationQuality(ctx, kCGInterpolationNone);
	CGColorSpaceRef rgb = CGColorSpaceCreateDeviceRGB();
	CGDataProviderRef provider = CGDataProviderCreateWithData(
		NULL,
		buf,
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

	CGContextDrawImage(ctx, d_app.view.frame, img);

	CGColorSpaceRelease(rgb);
	CGDataProviderRelease(provider);
	CGImageRelease(img);

}

@implementation DAppDelegate
- (void)applicationDidFinishLaunching:(NSNotification*)noti {

	DView* view = [[DView alloc] init];
	d_app.view = view;

	NSWindow* window = [[NSWindow alloc]
		initWithContentRect:NSMakeRect(0, 0, d_app.width, d_app.height)
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
	[window setRestorable:YES];
	[window center];
	[window setDelegate:[[DWindowDelegate alloc] init]];
	[window setContentView:view];
	[window makeFirstResponder:view];
	[window makeKeyAndOrderFront:nil];

	[NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
	[NSApp activateIgnoringOtherApps:YES];

#if defined(D_GL)

	NSOpenGLContext* ctx = [view openGLContext];

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

	if (d_app.desc.fullscreen) {
		d_app_set_fullscreen(true);
	}

	d_app_init();

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
	d_app.width = size.width;
	d_app.height = size.height;
	d_app.resized = true;
}
@end

@implementation DView
// - (BOOL)isOpaque {
//	return YES;
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
		d_app.key_states[k] = event.ARepeat ? D_BTN_RPRESSED: D_BTN_PRESSED;
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
	d_app.wheel = d_vec2f(event.scrollingDeltaX, event.scrollingDeltaY);
}
- (void)drawRect:(NSRect)rect {

	NSPoint ompos = [d_app.window mouseLocationOutsideOfEventStream];
	d_vec2 mpos = d_vec2f(
		ompos.x,
		d_app.height - ompos.y
	);
	d_app.mouse_dpos = d_vec2_sub(mpos, d_app.mouse_pos);
	d_app.mouse_pos = mpos;

	d_app_frame();

	if (d_app.quit) {
		[NSApp terminate:nil];
	}

}
@end

static void d_cocoa_run(d_app_desc* desc) {
	[NSApplication sharedApplication];
	[NSApp setDelegate:[[DAppDelegate alloc] init]];
	[NSApp run];
}

#endif // D_COCOA

// -------------------------------------------------------------
// UIKit
#if defined(D_UIKIT)

static void d_uikit_touch(d_btn_state state, NSSet<UITouch*>* tset, UIEvent* event) {

	NSArray<UITouch*>* touches = [tset allObjects];

	if ([touches count] == 1) {
		UITouch* t = touches[0];
		CGPoint pos = [t locationInView:[t view]];
		d_app.mouse_states[D_MOUSE_LEFT] = state;
		d_vec2 mpos = d_vec2f(
			pos.x * d_app.width / d_app.width,
			pos.y * d_app.height / d_app.height
		);
		if (d_app.touching) {
			d_app.mouse_dpos = d_vec2_sub(mpos, d_app.mouse_pos);
		}
		d_app.mouse_pos = mpos;
		if (state == D_BTN_PRESSED || state == D_BTN_RPRESSED || state == D_BTN_DOWN) {
			d_app.touching = true;
		} else if (state == D_BTN_RELEASED) {
			d_app.touching = false;
		}
	}

	for (UITouch* touch in touches) {
		uintptr_t id = (uintptr_t)touch;
		CGPoint cpos = [touch locationInView:[touch view]];
		d_vec2 pos = d_vec2f(cpos.x, cpos.y);
		switch (state) {
			case D_BTN_PRESSED:
				if (d_app.num_touches < D_MAX_TOUCHES) {
					d_app.touches[d_app.num_touches++] = (d_touch_state) {
						.id = id,
						.pos = pos,
						.dpos = d_vec2f(0.0, 0.0),
						.state = D_BTN_PRESSED,
					};
				}
				break;
			case D_BTN_DOWN:
				for (int i = 0; i < d_app.num_touches; i++) {
					d_touch_state* t = &d_app.touches[i];
					if (t->id == id) {
						t->dpos = d_vec2_sub(pos, t->pos);
						t->pos = pos;
					}
				}
				break;
			case D_BTN_RELEASED:
				for (int i = 0; i < d_app.num_touches; i++) {
					d_touch_state* t = &d_app.touches[i];
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

static void d_uikit_present(int w, int h, d_color* buf) {

	CGContextRef ctx = UIGraphicsGetCurrentContext();
	CGContextSetInterpolationQuality(ctx, kCGInterpolationNone);
	CGColorSpaceRef rgb = CGColorSpaceCreateDeviceRGB();
	CGDataProviderRef provider = CGDataProviderCreateWithData(
		NULL,
		buf,
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

	CGRect rect = d_app.view.frame;

	// TODO: why is it up side down
	CGContextTranslateCTM(ctx, 0, rect.size.height);
	CGContextScaleCTM(ctx, 1.0, -1.0);
	CGContextDrawImage(ctx, rect, img);

	CGColorSpaceRelease(rgb);
	CGDataProviderRelease(provider);
	CGImageRelease(img);

}

@implementation DView
- (void)drawRect:(CGRect)rect {
	d_app_frame();
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
	UIWindow* window = [[UIWindow alloc] initWithFrame:screen_rect];
	d_app.window = window;
	d_app.width = screen_rect.size.width;
	d_app.height = screen_rect.size.height;

	UIViewController* view_ctrl = [[UIViewController alloc] init];
	DView* view = [[DView alloc] init];
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

static void d_uikit_run(d_app_desc* desc) {
	UIApplicationMain(
		1,
		(char*[]){ "dirty" },
		nil,
		NSStringFromClass([DAppDelegate class])
	);
}

#endif // D_UIKIT

// -------------------------------------------------------------
// Term
#if defined(D_TERM)

#include <termios.h>
#include <unistd.h>
#include <poll.h>
#include <signal.h>
#include <sys/ioctl.h>

#define CSI "\x1b["
#define TERM_SHOW_CURSOR              CSI "?25h"
#define TERM_HIDE_CURSOR              CSI "?25l"
#define TERM_RESET_CURSOR             CSI "H"
#define TERM_RESET_COLOR              CSI "0m"
#define TERM_CLEAR_SCREEN             CSI "2J"
#define TERM_CLEAR_LINE               CSI "2K"
#define TERM_ENTER_ALTERNATIVE_SCREEN CSI "?1049h"
#define TERM_EXIT_ALTERNATIVE_SCREEN  CSI "?1049l"
#define TERM_TRUE_COLOR_FG            CSI "38;2;%d;%d;%dm"
#define TERM_TRUE_COLOR_BG            CSI "48;2;%d;%d;%dm"

static void d_term_cleanup(void) {
	printf(TERM_CLEAR_SCREEN);
	printf(TERM_EXIT_ALTERNATIVE_SCREEN);
	printf(TERM_SHOW_CURSOR);
	printf(TERM_RESET_COLOR);
}

static void d_term_sigint(int sig) {
	d_term_cleanup();
	exit(EXIT_SUCCESS);
}

static d_key d_term_key(char* c, int size) {
	switch (size) {
		case 3:
			switch (c[1]) {
				case '[':
					switch (c[2]) {
						case 'A': return D_KEY_UP;
						case 'B': return D_KEY_DOWN;
						case 'C': return D_KEY_RIGHT;
						case 'D': return D_KEY_LEFT;
					}
					break;
				case 'O':
					switch (c[2]) {
						case 'P': return D_KEY_F1;
						case 'Q': return D_KEY_F2;
						case 'R': return D_KEY_F3;
						case 'S': return D_KEY_F4;
					}
					break;
			}
			break;
		case 1:
			switch (c[0]) {
				case 'q': return D_KEY_Q;
				case 'w': return D_KEY_W;
				case 'e': return D_KEY_E;
				case 'r': return D_KEY_R;
				case 't': return D_KEY_T;
				case 'y': return D_KEY_Y;
				case 'u': return D_KEY_U;
				case 'i': return D_KEY_I;
				case 'o': return D_KEY_O;
				case 'p': return D_KEY_P;
				case 'a': return D_KEY_A;
				case 's': return D_KEY_S;
				case 'd': return D_KEY_D;
				case 'f': return D_KEY_F;
				case 'g': return D_KEY_G;
				case 'h': return D_KEY_H;
				case 'j': return D_KEY_J;
				case 'k': return D_KEY_K;
				case 'l': return D_KEY_L;
				case 'z': return D_KEY_Z;
				case 'x': return D_KEY_X;
				case 'c': return D_KEY_C;
				case 'v': return D_KEY_V;
				case 'b': return D_KEY_B;
				case 'n': return D_KEY_N;
				case 'm': return D_KEY_M;
				case '1': return D_KEY_1;
				case '2': return D_KEY_2;
				case '3': return D_KEY_3;
				case '4': return D_KEY_4;
				case '5': return D_KEY_5;
				case '6': return D_KEY_6;
				case '7': return D_KEY_7;
				case '8': return D_KEY_8;
				case '9': return D_KEY_9;
				case '0': return D_KEY_0;
				case '`': return D_KEY_BACKTICK;
				case ',': return D_KEY_COMMA;
				case '.': return D_KEY_PERIOD;
				case '=': return D_KEY_EQUAL;
				case '-': return D_KEY_MINUS;
				case '/': return D_KEY_SLASH;
				case ';': return D_KEY_SEMICOLON;
				case '\\': return D_KEY_BACKSLASH;
				case '\'': return D_KEY_QUOTE;
				case '[': return D_KEY_LBRACKET;
				case ']': return D_KEY_RBRACKET;
				case '(': return D_KEY_LPAREN;
				case ')': return D_KEY_RPAREN;
				case ' ': return D_KEY_SPACE;
				case 1: return D_KEY_ENTER;
				case 9: return D_KEY_TAB;
				case 27: return D_KEY_ESC;
				case 127: return D_KEY_BACKSPACE;
			}
			break;
	}
	return D_KEY_NONE;
}

static void d_term_run(d_app_desc* desc) {

	// raw mode
	struct termios attrs;
	tcgetattr(STDIN_FILENO, &attrs);
	attrs.c_lflag &= ~(ECHO | ICANON);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &attrs);
	printf(TERM_ENTER_ALTERNATIVE_SCREEN);
	printf(TERM_HIDE_CURSOR);
	atexit(d_term_cleanup);
	signal(SIGINT, d_term_sigint);

	d_app_init();

	struct pollfd pfd = {
		.fd = STDIN_FILENO,
		.events = POLLIN,
	};

	while (!d_app.quit) {

		if (poll(&pfd, 1, 0) > 0) {
			if (pfd.revents == POLLIN) {
				char c[4];
				int size = read(STDIN_FILENO, c, 4);
				d_key k = d_term_key(c, size);
				if (k) {
					d_app.key_states[k] = D_BTN_PRESSED;
				}
			}
		}

		d_app_frame();
		// TODO: sleep based on dt
		usleep(16666);

	}

}

static void d_term_present(int w, int h, d_color* buf) {

	printf(TERM_CLEAR_SCREEN);
	printf(TERM_RESET_CURSOR);

	int tw = w;
	int th = h;
	struct winsize term_size;

	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &term_size) != -1 && term_size.ws_col != 0) {
		tw = d_mini(w, term_size.ws_col);
		th = d_mini(h, term_size.ws_row * 2 - 2);
	}

	d_color prev_c1 = d_colorx(0x00000000);
	d_color prev_c2 = d_colorx(0x00000000);

	for (int y = 0; y < th; y += 2) {
		for (int x = 0; x < tw; x++) {
			d_color c1 = buf[y * w + x];
			d_color c2 = buf[(y + 1) * w + x];
			if (!d_color_eq(c1, prev_c1)) {
				printf(TERM_TRUE_COLOR_FG, c1.r, c1.g, c1.b);
				prev_c1 = c1;
			}
			if (!d_color_eq(c2, prev_c2)) {
				printf(TERM_TRUE_COLOR_BG, c2.r, c2.g, c2.b);
				prev_c2 = c2;
			}
			printf("\u2580");
		}
		printf("\n");
	}

	printf(TERM_RESET_COLOR);

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

// TODO: better way to scale?
static void d_x11_present(int w, int h, d_color* buf) {

	d_color* buf2 = malloc(d_app.width * d_app.height * sizeof(d_color));

	for (int x = 0; x < d_app.width; x++) {
		for (int y = 0; y < d_app.height; y++) {
			int xx = x * w / d_app.width;
			int yy = y * h / d_app.height;
			d_color c = buf[yy * w + xx];
			// TODO: it's drawing in BGRA
			buf2[y * d_app.width + x] = d_colori(c.b, c.g, c.r, c.a);
		}
	}

	XImage* ximg = XCreateImage(
		d_app.display,
		d_app.visual,
		d_app.depth,
		ZPixmap,
		0,
		(char*)buf2,
		d_app.width,
		d_app.height,
		32,
		0
	);

	// TODO: this is slow af
	XPutImage(
		d_app.display,
		d_app.window,
		d_app.gc,
		ximg, 0, 0, 0, 0,
		d_app.width,
		d_app.height
	);

	XFree(ximg);
	free(buf2);

}

static void d_x11_run(d_app_desc* desc) {

	Display* dis = XOpenDisplay(NULL);
	d_app.display = dis;
	int screen = XDefaultScreen(dis);
	Visual* visual = XDefaultVisual(dis, screen);
	d_app.visual = visual;
	unsigned int depth = XDefaultDepth(dis, screen);
	d_app.depth = depth;
	XSetWindowAttributes attrs;

	Window window = XCreateWindow(
		dis,
		XDefaultRootWindow(dis),
		0,
		0,
		d_app.width,
		d_app.height,
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

	XVisualInfo* glxvis = glXChooseVisual(dis, screen, glx_attrs);
	GLXContext glx = glXCreateContext(dis, glxvis, NULL, GL_TRUE);
	glXMakeCurrent(dis, window, glx);

	d_gl_init();

#elif defined(D_CPU)

	GC gc = XDefaultGC(dis,screen);
	d_app.gc = gc;

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
					d_app.mouse_pos = d_vec2f(
						event.xmotion.x * d_app.width / d_app.width,
						event.xmotion.y * d_app.height / d_app.height
					);
					break;
				case ClientMessage:
					d_app.quit = true;
					break;
			}
		}

		d_app_frame();

	}

	XDestroyWindow(dis, window);
	XCloseDisplay(dis);

}

#endif // D_X11

// -------------------------------------------------------------
// Windows
#if defined(D_WIN32)

static void d_win32_present(int w, int h, d_color *buf) {

	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(d_app.window, &ps);

	BITMAPINFO bmi = {
		.bmiHeader = {
			.biSize = sizeof(BITMAPINFOHEADER),
			.biWidth = w,
			.biHeight = -h,
			.biPlanes = 1,
			.biBitCount = 32,
			.biCompression = BI_BITFIELDS,
		},
	};

	DWORD* cmask = (DWORD*)bmi.bmiColors;

	cmask[0] = 0x000000FF;
	cmask[1] = 0x0000FF00;
	cmask[2] = 0x00FF0000;

	StretchDIBits(
		hdc,
		0, 0, d_app.width, d_app.height,
		0, 0, w, h,
		(unsigned char*)buf, &bmi,
		DIB_RGB_COLORS, SRCCOPY
	);

	EndPaint(d_app.window, &ps);

}

static d_key d_win32_key(int k) {
	switch (k) {
		case 0x00B: return D_KEY_0;
		case 0x002: return D_KEY_1;
		case 0x003: return D_KEY_2;
		case 0x004: return D_KEY_3;
		case 0x005: return D_KEY_4;
		case 0x006: return D_KEY_5;
		case 0x007: return D_KEY_6;
		case 0x008: return D_KEY_7;
		case 0x009: return D_KEY_8;
		case 0x00A: return D_KEY_9;
		case 0x01E: return D_KEY_A;
		case 0x030: return D_KEY_B;
		case 0x02E: return D_KEY_C;
		case 0x020: return D_KEY_D;
		case 0x012: return D_KEY_E;
		case 0x021: return D_KEY_F;
		case 0x022: return D_KEY_G;
		case 0x023: return D_KEY_H;
		case 0x017: return D_KEY_I;
		case 0x024: return D_KEY_J;
		case 0x025: return D_KEY_K;
		case 0x026: return D_KEY_L;
		case 0x032: return D_KEY_M;
		case 0x031: return D_KEY_N;
		case 0x018: return D_KEY_O;
		case 0x019: return D_KEY_P;
		case 0x010: return D_KEY_Q;
		case 0x013: return D_KEY_R;
		case 0x01F: return D_KEY_S;
		case 0x014: return D_KEY_T;
		case 0x016: return D_KEY_U;
		case 0x02F: return D_KEY_V;
		case 0x011: return D_KEY_W;
		case 0x02D: return D_KEY_X;
		case 0x015: return D_KEY_Y;
		case 0x02C: return D_KEY_Z;
		case 0x028: return D_KEY_QUOTE;
		case 0x02B: return D_KEY_BACKSLASH;
		case 0x033: return D_KEY_COMMA;
		case 0x00D: return D_KEY_EQUAL;
		case 0x029: return D_KEY_BACKTICK;
		case 0x01A: return D_KEY_LBRACKET;
		case 0x00C: return D_KEY_MINUS;
		case 0x034: return D_KEY_PERIOD;
		case 0x01B: return D_KEY_RBRACKET;
		case 0x027: return D_KEY_SEMICOLON;
		case 0x035: return D_KEY_SLASH;
		case 0x00E: return D_KEY_BACKSPACE;
		case 0x153: return D_KEY_DELETE;
		case 0x01C: return D_KEY_ENTER;
		case 0x001: return D_KEY_ESC;
		case 0x147: return D_KEY_HOME;
		case 0x152: return D_KEY_INSERT;
		case 0x15D: return D_KEY_MENU;
		case 0x151: return D_KEY_PAGE_DOWN;
		case 0x149: return D_KEY_PAGE_UP;
		case 0x039: return D_KEY_SPACE;
		case 0x00F: return D_KEY_TAB;
		case 0x03B: return D_KEY_F1;
		case 0x03C: return D_KEY_F2;
		case 0x03D: return D_KEY_F3;
		case 0x03E: return D_KEY_F4;
		case 0x03F: return D_KEY_F5;
		case 0x040: return D_KEY_F6;
		case 0x041: return D_KEY_F7;
		case 0x042: return D_KEY_F8;
		case 0x043: return D_KEY_F9;
		case 0x044: return D_KEY_F10;
		case 0x057: return D_KEY_F11;
		case 0x058: return D_KEY_F12;
		case 0x038: return D_KEY_LALT;
		case 0x01D: return D_KEY_LCTRL;
		case 0x02A: return D_KEY_LSHIFT;
		case 0x15B: return D_KEY_LMETA;
		case 0x138: return D_KEY_RALT;
		case 0x11D: return D_KEY_RCTRL;
		case 0x036: return D_KEY_RSHIFT;
		case 0x136: return D_KEY_RSHIFT;
		case 0x15C: return D_KEY_RMETA;
		case 0x150: return D_KEY_DOWN;
		case 0x14B: return D_KEY_LEFT;
		case 0x14D: return D_KEY_RIGHT;
		case 0x148: return D_KEY_UP;
	}
	return D_KEY_NONE;
}

LRESULT CALLBACK d_win32_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN: {
			int code = (int)(HIWORD(lParam) & 0x1FF);
			bool repeat = !!(lParam & 0x40000000);
			d_key k = d_win32_key(code);
			if (k) {
				d_app.key_states[k] = repeat ? D_BTN_RPRESSED : D_BTN_PRESSED;
			}
			break;
		}
		case WM_KEYUP:
		case WM_SYSKEYUP: {
			int code = (int)(HIWORD(lParam) & 0x1FF);
			d_key k = d_win32_key(code);
			if (k) {
				d_app.key_states[k] = D_BTN_RELEASED;
			}
			break;
		}
		case WM_LBUTTONDOWN:
			d_app.mouse_states[D_MOUSE_LEFT] = D_BTN_PRESSED;
			break;
		case WM_RBUTTONDOWN:
			d_app.mouse_states[D_MOUSE_RIGHT] = D_BTN_PRESSED;
			break;
		case WM_MBUTTONDOWN:
			d_app.mouse_states[D_MOUSE_MIDDLE] = D_BTN_PRESSED;
			break;
		case WM_LBUTTONUP:
			d_app.mouse_states[D_MOUSE_LEFT] = D_BTN_RELEASED;
			break;
		case WM_RBUTTONUP:
			d_app.mouse_states[D_MOUSE_RIGHT] = D_BTN_RELEASED;
			break;
		case WM_MBUTTONUP:
			d_app.mouse_states[D_MOUSE_MIDDLE] = D_BTN_RELEASED;
			break;
		case WM_MOUSEMOVE: {
			d_vec2 mpos = d_vec2f(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			d_app.mouse_dpos = d_vec2_sub(mpos, d_app.mouse_pos);
			d_app.mouse_pos = mpos;
			break;
		}
		case WM_MOUSEWHEEL:
			d_app.wheel.y = (float)((SHORT)HIWORD(wParam));
			break;
		case WM_MOUSEHWHEEL:
			d_app.wheel.x = -(float)((SHORT)HIWORD(wParam));
			break;
		case WM_CHAR:
			d_app.char_input = (char)wParam;
			break;
		case WM_SIZE:
			d_app.width = LOWORD(lParam);
			d_app.height = HIWORD(lParam);
			d_app.resized = true;
			break;
		case WM_SETFOCUS:
			d_app.has_focus = true;
			break;
		case WM_KILLFOCUS:
			d_app.has_focus = false;
			break;
		case WM_TIMER:
			// TODO: should we use WM_PAINT?
			InvalidateRect(d_app.window, NULL, TRUE);
			d_app_frame();
			break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

static void d_win32_run(d_app_desc* desc) {

	char CLASS_NAME[] = "dirty";

	HINSTANCE mod = GetModuleHandle(NULL);

	WNDCLASS wc = {
		.lpfnWndProc = d_win32_proc,
		.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
		.hInstance = mod,
		.hCursor = LoadCursor(NULL, IDC_ARROW),
		.hIcon = LoadIcon(NULL, IDI_WINLOGO),
		.lpszClassName = CLASS_NAME,
	};

	RegisterClass(&wc);

	d_app.window = CreateWindowEx(
		0,
		CLASS_NAME,
		desc->title,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		d_app.width, d_app.height,
		NULL, NULL, mod, NULL
	);

	if (d_app.window == NULL) {
		fprintf(stderr, "failed to create window\n");
		return;
	}

	// center the window
	int screen_width = GetSystemMetrics(SM_CXSCREEN);
	int screen_height = GetSystemMetrics(SM_CYSCREEN);
	int window_x = (screen_width - d_app.width) / 2;
	int window_y = (screen_height - d_app.height) / 2;

	SetWindowPos(
		d_app.window,
		NULL,
		window_x, window_y,
		0, 0,
		SWP_NOZORDER | SWP_NOSIZE
	);

	ShowWindow(d_app.window, SW_SHOW);
	DragAcceptFiles(d_app.window, 1);

	// TODO: too fast
	SetTimer(d_app.window, 1, USER_TIMER_MINIMUM, NULL);

	d_app_init();
	d_app_frame();

    bool done = false;

	while (!d_app.quit && !done) {
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				done = true;
				continue;
			} else {
				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}
		}
        d_app_frame();
	}

}

#endif // D_WIN32

// -------------------------------------------------------------
// Web
#if defined(D_CANVAS)

static bool streq(char* s1, char* s2) {
	return strcmp(s1, s2) == 0;
}

static d_key d_web_key(char* k, int loc) {

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
	else if (streq(k, "`")) return D_KEY_BACKTICK;
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

EMSCRIPTEN_KEEPALIVE void d_cjs_set_size(int w, int h) {
	d_app.width = w;
	d_app.height = h;
}

EMSCRIPTEN_KEEPALIVE void d_cjs_set_mouse_pos(float x, float y) {
	d_app.mouse_dpos = d_vec2_sub(d_vec2f(x, y), d_app.mouse_pos);
	d_app.mouse_pos = d_vec2f(x, y);
}

EMSCRIPTEN_KEEPALIVE void d_cjs_key_press(char* key, int loc, bool rep) {
	d_key k = d_web_key(key, loc);
	if (!k) {
		return;
	}
	d_app.key_states[k] = rep ? D_BTN_RPRESSED : D_BTN_PRESSED;
}

EMSCRIPTEN_KEEPALIVE void d_cjs_key_release(char* key, int loc) {
	d_key k = d_web_key(key, loc);
	if (!k) {
		return;
	}
	d_app.key_states[k] = D_BTN_RELEASED;
}

EMSCRIPTEN_KEEPALIVE void d_cjs_mouse_press(void) {
	d_app.mouse_states[D_MOUSE_LEFT] = D_BTN_PRESSED;
}

EMSCRIPTEN_KEEPALIVE void d_cjs_mouse_release(void) {
	d_app.mouse_states[D_MOUSE_LEFT] = D_BTN_RELEASED;
}

EMSCRIPTEN_KEEPALIVE void d_cjs_set_focus(bool f) {
	d_app.has_focus = f;
}

EM_JS(void, d_js_set_fullscreen, (bool b), {
	const canvas = dApp.canvas;
	if (b) {
		// TODO: becomes blurry
		dApp.originWidth = canvas.width;
		dApp.originHeight = canvas.height;
		canvas.width = window.innerWidth;
		canvas.height = window.innerHeight;
		dApp.isFullscreen = true;
	} else {
		canvas.width = dApp.originWidth || canvas.width;
		canvas.height = dApp.originHeight || canvas.height;
		dApp.isFullscreen = false;
	}
})

EM_JS(bool, d_js_is_fullscreen, (void), {
	return dApp.isFullscreen;
})

EM_JS(bool, d_js_set_title, (char* title), {
	document.title = UTF8ToString(title);
})

EM_JS(void, d_js_canvas_init, (char* root, int w, int h), {

	window.dApp = {};

	const name = UTF8ToString(root);
	const canvas = document.createElement("canvas");
	dApp.canvas = canvas;
	canvas.width = w;
	canvas.height = h;
	canvas.id = "dirty";
	canvas.style = "image-rendering: pixelated; image-rendering: crisp-edges;";

	if (name) {
		document.getElementById(name).appendChild(canvas);
	} else {
		document.body.appendChild(canvas);
	}

	const ctx = canvas.getContext("2d");

	ctx.imageSmoothingEnabled = false;

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
		ccall(
			"d_cjs_set_mouse_pos",
			"void",
			[ "number", "number" ],
			[ e.pageX - rect.left, e.pageY - rect.top ]
		);
	});

	document.addEventListener("keydown", (e) => {
		if (preventDefaultKeys.includes(e.key)) {
			e.preventDefault();
		}
		ccall(
			"d_cjs_key_press",
			"void",
			[ "string", "number", "bool" ],
			[ e.key, e.location, e.repeat ]
		);
	});

	document.addEventListener("keyup", (e) => {
		ccall(
			"d_cjs_key_release",
			"void",
			[ "string", "number" ],
			[ e.key, e.location ]
		);
	});

	document.addEventListener("mousedown", (e) => {
		ccall("d_cjs_mouse_press");
	});

	document.addEventListener("mouseup", (e) => {
		ccall("d_cjs_mouse_release");
	});

	document.addEventListener("visibilitychange", () => {
		if (document.visibilityState === "visible") {
			ccall(
				"d_cjs_set_focus",
				"void",
				[ "bool" ],
				[ true ]
			);
		} else {
			ccall(
				"d_cjs_set_focus",
				"void",
				[ "bool" ],
				[ false ]
			);
		}
	});


	// TODO: use js native time?
	function frame() {
		ccall("d_cjs_app_frame");
		requestAnimationFrame(frame);
	}

	requestAnimationFrame(frame);

})

EMSCRIPTEN_KEEPALIVE void d_cjs_app_frame(void) {
	d_app_frame();
}

EM_JS(void, d_canvas_present, (int w, int h, d_color* buf), {

	const canvas = dApp.canvas;
	const pixels = new Uint8ClampedArray(HEAPU8.buffer, buf, w * h * 4);
	const img = new ImageData(pixels, w, h);

	ccall(
		"d_cjs_set_size",
		"void",
		[ "number", "number" ],
		[ canvas.width, canvas.height ]
	);

	const ctx = canvas.getContext("2d");

	ctx.putImageData(img, 0, 0);
	ctx.setTransform(canvas.width / w, 0, 0, canvas.height / h, 0, 0);
	ctx.drawImage(canvas, 0, 0);

})

static void d_canvas_run(d_app_desc* desc) {
	d_js_canvas_init(desc->canvas_root, d_app.width, d_app.height);
	d_js_set_title(desc->title);
	d_app_init();
}

#endif // D_CANVAS

void d_app_run(d_app_desc desc) {

	d_app.desc = desc;
	d_app.width = desc.width ? desc.width : D_DEF_WIDTH;
	d_app.height = desc.height ? desc.height : D_DEF_HEIGHT;
	d_app.scale_mode = D_DEF_SCALEMODE;
	d_app.time = 0.0;
	d_app.dt = 0.0;
	d_app.mouse_pos = d_vec2f(0.0, 0.0);
	d_app.mouse_dpos = d_vec2f(0.0, 0.0);
	d_app.wheel = d_vec2f(0.0, 0.0);
	d_app.has_focus = true;
	gettimeofday(&d_app.start_time, NULL);

#if defined(D_COCOA)
	d_cocoa_run(&desc);
#elif defined(D_UIKIT)
	d_uikit_run(&desc);
#elif defined(D_X11)
	d_x11_run(&desc);
#elif defined(D_WIN32)
	d_win32_run(&desc);
#elif defined(D_CANVAS)
	d_canvas_run(&desc);
#elif defined(D_TERM)
	d_term_run(&desc);
#endif

}

void d_app_quit(void) {
	d_app.quit = true;
}

float d_app_time(void) {
	return d_app.time;
}

float d_app_dt(void) {
	return d_app.dt;
}

int d_app_fps(void) {
	return d_app.fps;
}

void d_app_set_fullscreen(bool b) {
#if defined(D_COCOA)
	if (b != d_app_is_fullscreen()) {
		[d_app.window toggleFullScreen:nil];
	}
#elif defined(D_X11)
	// TODO
#elif defined(D_CANVAS)
	d_js_set_fullscreen(b);
#endif
}

bool d_app_is_fullscreen(void) {
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
void d_app_set_mouse_locked(bool b) {
}

// TODO
bool d_app_is_mouse_locked(void) {
	return false;
}

// TODO
void d_app_set_mouse_hidden(bool b) {
}

// TODO
bool d_app_is_mouse_hidden(void) {
	return false;
}

void d_app_set_title(char* title) {
#if defined(D_COCOA)
	@autoreleasepool {
		[d_app.window setTitle:[NSString stringWithUTF8String:title]];
	}
#elif defined(D_X11)
	XStoreName(d_app.display, d_app.window, title);
#elif defined(D_WIN32)
	SetWindowText(d_app.window, title);
#elif defined(D_CANVAS)
	d_js_set_title(title);
#endif
}

bool d_app_is_keyboard_shown(void) {
	// TODO
	return false;
}

void d_app_set_keyboard_shown(bool b) {
#if defined(D_IOS)
	// TODO
#endif
}

bool d_app_key_pressed(d_key k) {
	return
		d_app.key_states[k] == D_BTN_PRESSED
		;
}

bool d_app_key_rpressed(d_key k) {
	return
		d_app.key_states[k] == D_BTN_PRESSED
		|| d_app.key_states[k] == D_BTN_RPRESSED
		;
}

bool d_app_key_down(d_key k) {
	return
		d_app.key_states[k] == D_BTN_PRESSED
		|| d_app.key_states[k] == D_BTN_RPRESSED
		|| d_app.key_states[k] == D_BTN_DOWN
		;
}

bool d_app_key_released(d_key k) {
	return d_app.key_states[k] == D_BTN_RELEASED;
}

bool d_app_key_mod(d_keymod kmod) {
	switch (kmod) {
		case D_KEYMOD_ALT:
			return d_app_key_down(D_KEY_LALT) || d_app_key_down(D_KEY_RALT);
		case D_KEYMOD_META:
			return d_app_key_down(D_KEY_LMETA) || d_app_key_down(D_KEY_RMETA);
		case D_KEYMOD_CTRL:
			return d_app_key_down(D_KEY_LCTRL) || d_app_key_down(D_KEY_RCTRL);
		case D_KEYMOD_SHIFT:
			return d_app_key_down(D_KEY_LSHIFT) || d_app_key_down(D_KEY_RSHIFT);
	}
	return false;
}

bool d_app_mouse_pressed(d_mouse k) {
	return d_app.mouse_states[k] == D_BTN_PRESSED;
}

bool d_app_mouse_released(d_mouse k) {
	return d_app.mouse_states[k] == D_BTN_RELEASED;
}

bool d_app_mouse_down(d_mouse k) {
	return d_app.mouse_states[k] == D_BTN_DOWN
		|| d_app.mouse_states[k] == D_BTN_PRESSED;
}

int d_app_width(void) {
	return d_app.width;
}

int d_app_height(void) {
	return d_app.height;
}

d_vec2 d_app_mouse_pos(void) {
	return d_app.mouse_pos;
}

d_vec2 d_app_mouse_dpos(void) {
	return d_app.mouse_dpos;
}

bool d_app_mouse_moved(void) {
	return d_app.mouse_dpos.x != 0.0 || d_app.mouse_dpos.y != 0.0;
}

bool d_app_touch_pressed(d_touch t) {
	if (t >= D_MAX_TOUCHES) {
		fprintf(stderr, "touch not found: %d\n", t);
		return false;
	}
	return d_app.touches[t].state == D_BTN_PRESSED;
}

bool d_app_touch_released(d_touch t) {
	if (t >= D_MAX_TOUCHES) {
		fprintf(stderr, "touch not found: %d\n", t);
		return false;
	}
	return d_app.touches[t].state == D_BTN_RELEASED;
}

bool d_app_touch_moved(d_touch t) {
	if (t >= D_MAX_TOUCHES) {
		fprintf(stderr, "touch not found: %d\n", t);
		return false;
	}
	return d_app.touches[t].dpos.x != 0.0 || d_app.touches[t].dpos.x != 0.0;
}

d_vec2 d_app_touch_pos(d_touch t) {
	if (t >= D_MAX_TOUCHES) {
		fprintf(stderr, "touch not found: %d\n", t);
		return d_vec2f(0, 0);
	}
	return d_app.touches[t].pos;
}

d_vec2 d_app_touch_dpos(d_touch t) {
	if (t >= D_MAX_TOUCHES) {
		fprintf(stderr, "touch not found: %d\n", t);
		return d_vec2f(0, 0);
	}
	return d_app.touches[t].dpos;
}

bool d_app_resized(void) {
	return d_app.resized;
}

bool d_app_scrolled(void) {
	return d_app.wheel.x != 0.0 || d_app.wheel.y != 0.0;
}

d_vec2 d_app_wheel(void) {
	return d_app.wheel;
}

char d_app_input(void) {
	return d_app.char_input;
}

bool d_app_has_focus(void) {
#if defined(D_COCOA)
	return [d_app.window isMainWindow];
#endif
	return d_app.has_focus;
}

void d_app_present(int w, int h, d_color* buf) {
#if defined(D_GL)
	d_gl_present(w, h, buf);
#elif defined(D_METAL)
	d_mtl_present(w, h, buf);
#elif defined(D_TERM)
	d_term_present(w, h, buf);
#elif defined(D_CPU)
	#if defined(D_COCOA)
		d_cocoa_present(w, h, buf);
	#elif defined(D_UIKIT)
		d_uikit_present(w, h, buf);
	#elif defined(D_WIN32)
		d_win32_present(w, h, buf);
	#elif defined(D_X11)
		d_x11_present(w, h, buf);
	#elif defined(D_CANVAS)
		d_canvas_present(w, h, buf);
	#endif
#endif
}

#endif // #ifndef D_APP_IMPL_ONCE
#endif // #ifdef D_APP_IMPL
