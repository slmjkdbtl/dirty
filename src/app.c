// wengwengweng

// TODO: scale strategies
// TODO: try CVDisplayLink?

#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>
#include <dirty/dirty.h>

#if !defined(D_CPU) && !defined(D_GL) && !defined(D_METAL)
#error "must define a blit method (D_CPU, D_GL, D_METAL)"
#endif

#if defined(D_METAL) && !defined(D_MACOS) && !defined(D_IOS)
#error "D_METAL is only for macOS or iOS"
#endif

#if !defined(D_COCOA) && !defined(D_UIKIT) && !defined(D_X11) && !defined(D_CANVAS)
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

void d_present(const color *canvas) {
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
#endif // D_METAL

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
	CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, d_app.buf, w * h * 4, NULL);

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

	if (d_app.desc.touch_is_mouse) {
		if ([touches count] == 1) {
			UITouch *t = touches[0];
			CGPoint pos = [t locationInView:[t view]];
			d_app.mouse_states[D_MOUSE_LEFT] = state;
			d_app.mouse_pos = vec2f(
				pos.x * d_app.width / d_app.win_width,
				pos.y * d_app.height / d_app.win_height
			);
		}
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
	// TODO
#elif defined(D_CPU)

	int w = d_width();
	int h = d_height();

	CGContextRef ctx = UIGraphicsGetCurrentContext();
	CGContextSetInterpolationQuality(ctx, kCGInterpolationNone);
	CGColorSpaceRef rgb = CGColorSpaceCreateDeviceRGB();
	CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, d_app.buf, w * h * 4, NULL);

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

	XSelectInput(dis, window, ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
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

#include <string.h>

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
	const img = new ImageData(new Uint8ClampedArray(HEAPU8.buffer, buf, w * h * 4), w, h);

	_d_cjs_set_win_size(canvas.width, canvas.height);

#if defined(D_CPU)

	const ctx = canvas.getContext("2d");

	ctx.putImageData(img, 0, 0, 0, 0, canvas.width, canvas.height);
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
	d_app.width = desc.width ? desc.width : 640;
	d_app.height = desc.height ? desc.height : 480;
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
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	[d_app.window setTitle:[NSString stringWithUTF8String:title]];
	[pool drain];
#elif defined(D_X11)
	XStoreName(d_app.display, d_app.window, title);
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

