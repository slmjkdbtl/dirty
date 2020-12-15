// wengwengweng

#define D_CPU
#define DIRTY_IMPL
#include "../dirty.h"

void frame() {
	d_draw_text("hi", vec2f(0, 0));
}

int main() {
	d_run((d_desc) {
		.title = "hi",
		.frame = frame,
	});
}

