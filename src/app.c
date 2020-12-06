// wengwengweng

#ifndef D_SOKOL

#import <Cocoa/Cocoa.h>

#define SOKOL_IMPL
#include <sokol/sokol_time.h>
#include <dirty/dirty.h>
#include <OpenGL/gl.h>

#define GL_SILENCE_DEPRECATION
#define NUM_TOUCHES 8

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
	color *buf;
#if defined(D_GL)
	GLuint gl_tex;
#endif
} d_app_t;

static d_app_t d_app;

@interface WindowDelegate:NSObject<NSWindowDelegate>
@end

@implementation WindowDelegate
- (void)windowWillClose:(NSNotification*)noti {
	[NSApp terminate:nil];
}
- (void)windowDidResize:(NSNotification*)noti {
	printf("resize\n");
}
- (void)windowDidMiniaturize:(NSNotification*)noti {
	printf("mini\n");
}
- (void)windowDidDeminiaturize:(NSNotification*)noti {
	printf("demini\n");
}
@end

@interface Window:NSWindow
@end

@implementation Window
- (void)keyDown:(NSEvent*)event {
	printf("key down\n");
}
- (void)keyUp:(NSEvent*)event {
	printf("key up\n");
}
- (void)mouseDown:(NSEvent*)event {
	printf("mouse down\n");
}
- (void)mouseUp:(NSEvent*)event {
	printf("mouse up\n");
}
- (void)rightMouseDown:(NSEvent*)event {
	printf("right mouse down\n");
}
- (void)rightMouseUp:(NSEvent*)event {
	printf("right mouse up\n");
}
- (void)mouseMoved:(NSEvent*)event {
// 	printf("mouse moved\n");
}
- (void)mouseEntered:(NSEvent*)event {
	printf("mouse entered\n");
}
- (void)mouseExited:(NSEvent*)event {
	printf("mouse exited\n");
}
- (void)scrollWheel:(NSEvent*)event {
	printf("wheel\n");
}
@end

#if defined(D_GL)
@interface View:NSOpenGLView
#else
@interface View:NSView
#endif
@end

@implementation View
- (void)drawRect:(NSRect)rect {

	if (!d_app.buf) {
		return;
	}

	int w = d_app.desc.width;
	int h = d_app.desc.height;

#if defined(D_GL)

	glClear(GL_COLOR_BUFFER_BIT);

	glBindTexture(GL_TEXTURE_2D, d_app.gl_tex);

	glTexSubImage2D(
		GL_TEXTURE_2D,
		0,
		0,
		0,
		w,
		h,
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

#elif defined(D_NOGPU)

	CGContextRef ctx = [[NSGraphicsContext currentContext] CGContext];
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

#endif

}
@end

void d_present(color *canvas) {
	d_app.buf = canvas;
}

void d_process_btn(d_btn *b) {
	if (*b == D_BTN_PRESSED || *b == D_BTN_RPRESSED) {
		*b = D_BTN_DOWN;
	} else if (*b == D_BTN_RELEASED) {
		*b = D_BTN_IDLE;
	}
}

static void poll() {
	@autoreleasepool {
		for (;;) {
			NSEvent *event = [NSApp
				nextEventMatchingMask:NSEventMaskAny
				untilDate:[NSDate distantPast]
				inMode:NSDefaultRunLoopMode
				dequeue:YES
			];
			if (event == nil) {
				break;
			}
			[NSApp sendEvent:event];
		}
	}
}

void d_run(d_desc desc) {

	d_app.desc = desc;
	int w = desc.width;
	int h = desc.height;

	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

	[NSApplication sharedApplication];
	[NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
	[NSApp activateIgnoringOtherApps:YES];

	NSUInteger window_style =
		NSWindowStyleMaskTitled
		| NSWindowStyleMaskClosable
		| NSWindowStyleMaskResizable
		| NSWindowStyleMaskMiniaturizable
		;

	NSWindow *window = [[Window alloc]
		initWithContentRect: NSMakeRect(0, 0, w * desc.scale, h * desc.scale)
		styleMask: window_style
		backing: NSBackingStoreBuffered
		defer: NO
	];

	[window autorelease];
	[window setAcceptsMouseMovedEvents:YES];
	[window setTitle:@"hi"];
	[window center];
	[window setContentView:[[View alloc] init]];
	[window makeFirstResponder:[window contentView]];
	[window setDelegate:[[WindowDelegate alloc] init]];
	[window makeKeyAndOrderFront:nil];

#if defined(D_GL)

	NSOpenGLContext* ctx = [[window contentView] openGLContext];
	GLint swapInt = 1;
	[ctx setValues:&swapInt forParameter:NSOpenGLContextParameterSwapInterval];
// 	[[window contentView] setWantsBestResolutionOpenGLSurface:YES];
	[ctx makeCurrentContext];

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glViewport(0, 0, 640, 480);
	glEnable(GL_TEXTURE_2D);

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
		desc.width,
		desc.height,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		NULL
	);

	glBindTexture(GL_TEXTURE_2D, 0);

#endif

	d_gfx_init(&d_app.desc);
	d_audio_init(&d_app.desc);
	d_fs_init(&d_app.desc);
	desc.init();
	stm_setup();

	while (1) {
		poll();
		desc.frame();
		d_gfx_frame_end();
		[[window contentView] setNeedsDisplay:YES];
		float time = stm_sec(stm_now());
		d_app.dt = time - d_app.time;
		d_app.time = time;
	}

	[pool drain];

}

void d_quit() {
	[NSApp terminate:nil];
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

void d_set_fullscreen(bool b) {
}

bool d_fullscreen() {
	return false;
}

void d_lock_mouse(bool b) {
}

bool d_mouse_locked() {
	return false;
}

void d_hide_mouse(bool b) {
}

bool d_mouse_hidden() {
	return false;
}

void d_show_keyboard(bool b) {
}

bool d_keyboard_shown() {
	return false;
}

void d_set_title(const char *title) {
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


#endif

