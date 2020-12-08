// wengwengweng

// TODO: scale strategies

#include <stdio.h>
#include <stdarg.h>
#define SOKOL_IMPL
#include <sokol/sokol_time.h>
#include <dirty/dirty.h>

#if !defined(D_CPU) && !defined(D_GL) && !defined(D_METAL) && !defined(D_WGPU) && !defined(D_D3D11)
#error "must define a blit method (D_CPU, D_GL, D_METAL, D_WGPU, D_D3D11)"
#endif

#if defined(D_METAL) && !defined(D_MACOS) && !defined(D_IOS)
#error "D_METAL is only on macOS or iOS"
#elif defined(D_WGPU) && !defined(D_WEB)
#error "D_WGPU is only on web"
#elif defined(D_D3D11) && !defined(D_WINDOWS)
#error "D_D3D11 is only on windows"
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

#if defined(D_MACOS)
	#import <Cocoa/Cocoa.h>
	#if defined(D_METAL)
		#import <Metal/Metal.h>
		#import <MetalKit/MetalKit.h>
	#endif
#elif defined(D_IOS)
	#import <UIKit/UIKit.h>
#elif defined(D_WEB)
	#include <emscripten/emscripten.h>
	#include <emscripten/html5.h>
#endif

#define D_NUM_TOUCHES 8

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
	uintptr_t id;
	vec2 pos;
	vec2 dpos;
	d_btn state;
} d_touch_state;

typedef struct {
	d_desc desc;
	color *buf;
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
	d_touch_state touches[D_NUM_TOUCHES];
	bool resized;
	char char_input;
	float fps_timer;
	int fps;
#if defined(D_GL)
	GLuint gl_tex;
#endif
} d_app_t;

static d_app_t d_app;

void d_present(color *canvas) {
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

	d_gfx_init(&d_app.desc);
	d_audio_init(&d_app.desc);
	d_fs_init(&d_app.desc);
	stm_setup();

	if (d_app.desc.init) {
		d_app.desc.init();
	}

}

static void d_app_frame_end() {

	// time
	float time = stm_sec(stm_now());

	d_app.dt = time - d_app.time;
	d_app.time = time;

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

	for (int i = 0; i < D_NUM_TOUCHES; i++) {
		d_process_btn(&d_app.touches[i].state);
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

static void d_opengl_init() {

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

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

}

static void d_opengl_blit() {

	glClear(GL_COLOR_BUFFER_BIT);
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

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2f(-1, 1);
	glTexCoord2f(1, 0); glVertex2f(1, 1);
	glTexCoord2f(1, 1); glVertex2f(1, -1);
	glTexCoord2f(0, 1); glVertex2f(-1, -1);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glFlush();

}

#endif // D_GL

// -------------------------------------------------------------
// macOS
#if defined(D_MACOS)

static d_key d_macos_key(unsigned short k) {
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

@interface DAppDelegate : NSObject<NSApplicationDelegate>
-(void) drawLoop:(NSTimer*) timer;
@end

@implementation DAppDelegate
- (void)applicationDidFinishLaunching:(NSNotification*)noti {
	[NSTimer scheduledTimerWithTimeInterval:0.001 target:self selector:@selector(drawLoop:) userInfo:nil repeats:YES];
}
-(void) drawLoop:(NSTimer*) timer {
	[[[NSApp mainWindow] contentView] setNeedsDisplay:YES];
}
@end

@interface DWindowDelegate : NSObject<NSWindowDelegate>
@end

@implementation DWindowDelegate
- (void)windowWillClose:(NSNotification*)noti {
	[NSApp terminate:nil];
}
- (void)windowDidResize:(NSNotification*)noti {
	NSSize size = [[[NSApp mainWindow] contentView] frame].size;
	d_app.win_width = size.width;
	d_app.win_height = size.height;
	d_app.resized = true;
}
@end

#if defined(D_GL)
@interface DView : NSOpenGLView
#elif defined(D_METAL)
@interface DView : MTKView
#elif defined(D_CPU)
@interface DView : NSView
#endif
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
	d_key k = d_macos_key(event.keyCode);
	if (k) {
		if (event.ARepeat) {
			d_app.key_states[k] = D_BTN_RPRESSED;
		} else {
			d_app.key_states[k] = D_BTN_PRESSED;
		}
	}
}
- (void)keyUp:(NSEvent*)event {
	d_key k = d_macos_key(event.keyCode);
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

	NSPoint ompos = [[NSApp mainWindow] mouseLocationOutsideOfEventStream];
	vec2 mpos = vec2f(
		ompos.x * d_app.width / d_app.win_width,
		d_app.height - ompos.y * d_app.height / d_app.win_height
	);
	d_app.mouse_dpos = vec2_sub(mpos, d_app.mouse_pos);
	d_app.mouse_pos = mpos;

	if (d_app.desc.frame) {
		d_app.desc.frame();
	}

	d_gfx_frame_end();

	if (!d_app.buf) {
		return;
	}

#if defined(D_GL)
	d_opengl_blit();
#elif defined(D_METAL)
	// TODO
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

	d_app_frame_end();

}
@end

static void d_macos_run(const d_desc *desc) {

	[NSApplication sharedApplication];
	[NSApp setDelegate:[[DAppDelegate alloc] init]];
	[NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
	[NSApp activateIgnoringOtherApps:YES];

	NSWindow *window = [[NSWindow alloc]
		initWithContentRect: NSMakeRect(0, 0, d_app.win_width, d_app.win_height)
		styleMask:
			NSWindowStyleMaskTitled
			| NSWindowStyleMaskClosable
			| NSWindowStyleMaskResizable
			| NSWindowStyleMaskMiniaturizable
		backing: NSBackingStoreBuffered
		defer: NO
	];

	if (desc->title) {
		[window setTitle:[NSString stringWithUTF8String:desc->title]];
	}

	[window setAcceptsMouseMovedEvents:YES];
	[window center];
	[window setDelegate:[[DWindowDelegate alloc] init]];
	[window makeKeyAndOrderFront:nil];

	DView *view = [[DView alloc] init];
	[window setContentView:view];
	[window makeFirstResponder:view];

#if defined(D_GL)

	NSOpenGLContext* ctx = [view openGLContext];

	if (desc->hidpi) {
		[[window contentView] setWantsBestResolutionOpenGLSurface:YES];
	}

	[ctx setValues:&desc->vsync forParameter:NSOpenGLContextParameterSwapInterval];
	[ctx makeCurrentContext];

	d_opengl_init();

#elif defined(D_METAL)

	// TODO
	view.preferredFramesPerSecond = 60;

#endif // D_METAL

	d_app_init();

	[NSApp run];

}

#endif // D_MACOS

// -------------------------------------------------------------
// iOS
#if defined(D_IOS)

static void d_ios_touch(d_btn state, NSSet<UITouch*> *touches, UIEvent *event) {

	if (d_app.desc.touch_is_mouse) {
		int cnt = [touches count];
		if (cnt == 1) {
			d_app.mouse_states[D_MOUSE_LEFT] = state;
		} else if (cnt == 2) {
			d_app.mouse_states[D_MOUSE_RIGHT] = state;
		}
	}

	for (UITouch *touch in touches) {
		uintptr_t id = (uintptr_t)touch;
		CGPoint pos = [touch locationInView:[touch view]];
		if (state == D_BTN_PRESSED) {
			d_touch_state t = (d_touch_state) {
				.id = id,
				.pos = vec2f(pos.x, pos.y),
				.dpos = vec2f(0.0, 0.0),
				.state = state,
			};
		}
		// TODO
	}

}

#if defined(D_CPU)
@interface DView : UIView
#elif defined(D_METAL)
@interface DView : MTKView
#endif
@end

@implementation DView
- (void)drawRect:(CGRect)rect {

	int w = d_width();
	int h = d_height();

	d_img dimg = d_make_img(w, h);
	d_img_fill(&dimg, colori(0, 255, 255, 255));

	CGContextRef ctx = UIGraphicsGetCurrentContext();
	CGContextSetInterpolationQuality(ctx, kCGInterpolationNone);
	CGColorSpaceRef rgb = CGColorSpaceCreateDeviceRGB();
	CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, dimg.pixels, w * h * 4, NULL);

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
	d_free_img(&dimg);

	d_app_frame_end();

}

- (void)touchesBegan:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
	d_ios_touch(D_BTN_PRESSED, touches, event);
}
- (void)touchesMoved:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
	d_ios_touch(D_BTN_DOWN, touches, event);
}
- (void)touchesEnded:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
	d_ios_touch(D_BTN_RELEASED, touches, event);
}
- (void)touchesCancelled:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
	d_ios_touch(D_BTN_RELEASED, touches, event);
}
@end

@interface DAppDelegate : NSObject<UIApplicationDelegate>
@property (strong, nonatomic) UIWindow *window;
@end

@implementation DAppDelegate
- (BOOL)application:(UIApplication*)app didFinishLaunchingWithOptions:(NSDictionary*)opt {

	// TODO: nothing drawn

	CGRect screen_rect = [[UIScreen mainScreen] bounds];
	UIWindow *window = [[UIWindow alloc] initWithFrame:screen_rect];
	d_app.win_width = screen_rect.size.width;
	d_app.win_height = screen_rect.size.height;

	UIViewController *view_ctrl = [[UIViewController alloc] init];
	DView *view = [[DView alloc] init];
	view_ctrl.view = view;
	window.rootViewController = view_ctrl;
	window.backgroundColor = [UIColor whiteColor];
	[window makeKeyAndVisible];
	self.window = window;

	d_app_init();

	return YES;

}
@end

static void d_ios_run(const d_desc *desc) {
	UIApplicationMain(0, nil, nil, NSStringFromClass([DAppDelegate class]));
}

#endif // D_IOS


// -------------------------------------------------------------
// Web
#if defined(D_WEB)

EM_JS(void, d_web_init, (const d_desc *desc), {
	// TODO
});

void d_web_run(const d_desc *desc) {
// 	emscripten_set_main_loop(func, 60, true);
}

#endif // D_WEB

void d_run(d_desc desc) {

	d_app.desc = desc;
	float scale = desc.scale ? desc.scale : 1.0;
	d_app.width = desc.width ? desc.width : 640;
	d_app.height = desc.height ? desc.height : 480;
	d_app.win_width = d_app.width * scale;
	d_app.win_height = d_app.height * scale;

#if defined(D_MACOS)
	d_macos_run(&desc);
#elif defined(D_IOS)
	d_ios_run(&desc);
#elif defined(D_WEB)
	d_web_run(&desc);
#endif

}

void d_quit() {
#if defined(D_MACOS)
	[NSApp terminate:nil];
#endif
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
#if defined(D_MACOS)
	if (b != d_fullscreen()) {
		[[NSApp mainWindow] toggleFullScreen:nil];
	}
#endif
}

bool d_fullscreen() {
#if defined(D_MACOS)
	return [[NSApp mainWindow] styleMask] & NSWindowStyleMaskFullScreen;
#elif defined(D_IOS)
	return true;
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
#if defined(D_MACOS)
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	[[NSApp mainWindow] setTitle:[NSString stringWithUTF8String:title]];
	[pool drain];
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
	d_assert(t < D_NUM_TOUCHES, "touch not found: %d\n", t);
	return d_app.touches[t].state == D_BTN_PRESSED;
}

bool d_touch_released(d_touch t) {
	d_assert(t < D_NUM_TOUCHES, "touch not found: %d\n", t);
	return d_app.touches[t].state == D_BTN_RELEASED;
}

bool d_touch_moved(d_touch t) {
	d_assert(t < D_NUM_TOUCHES, "touch not found: %d\n", t);
	return d_app.touches[t].dpos.x != 0.0 || d_app.touches[t].dpos.x != 0.0;
}

vec2 d_touch_pos(d_touch t) {
	d_assert(t < D_NUM_TOUCHES, "touch not found: %d\n", t);
	return d_app.touches[t].pos;
}

vec2 d_touch_dpos(d_touch t) {
	d_assert(t < D_NUM_TOUCHES, "touch not found: %d\n", t);
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
#if defined(D_MACOS)
	return [[NSApp mainWindow] isMainWindow];
#endif
}

