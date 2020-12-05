// wengwengweng

#import <Cocoa/Cocoa.h>

#include <sokol/sokol_time.h>
#include <dirty/dirty.h>

typedef struct {
	d_desc desc;
	color *buf;
	float time;
	float dt;
	int width;
	int height;
} d_app_t;

static d_app_t d_app2;

@interface WindowDelegate:NSObject<NSWindowDelegate>
@end

@implementation WindowDelegate
- (BOOL)windowShouldClose:(NSWindow*)sender {
	return YES;
}
- (void)windowWillClose:(NSNotification*)noti {
	[NSApp terminate:self];
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

@interface View:NSView
@end

void d_present2(color *canvas) {
	d_app2.buf = canvas;
}

@implementation View
- (void)drawRect:(NSRect)rect {

	if (!d_app2.buf) {
		return;
	}

	int w = d_app2.desc.width;
	int h = d_app2.desc.height;

	CGContextRef ctx = [[NSGraphicsContext currentContext] CGContext];
	CGColorSpaceRef rgb = CGColorSpaceCreateDeviceRGB();
	CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, d_app2.buf, w * h * 4, NULL);

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

}
@end

void poll() {
	@autoreleasepool {
		for (;;) {
			NSEvent* event = [NSApp
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

void d_run2(d_desc desc) {

	d_app2.desc = desc;
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

	d_gfx_init(&d_app2.desc);
	d_audio_init(&d_app2.desc);
	d_fs_init(&d_app2.desc);
	desc.init();
	stm_setup();

	while (1) {
		poll();
		desc.frame();
		d_gfx_frame_end();
		float time = stm_sec(stm_now());
		d_app2.dt = time - d_app2.time;
		d_app2.time = time;
		[[window contentView] setNeedsDisplay:YES];
	}

	[pool drain];

}

float d_dt2() {
	return d_app2.dt;
}

