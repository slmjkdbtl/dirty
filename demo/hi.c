// wengwengweng

#define D_CPU
#define DIRTY_IMPL
#include <dirty.h>

void frame() {
	d_clear();
	d_draw_text("hi", vec2f(0, 0));
}

int main() {
	d_run((d_desc) {
		.title = "hi",
		.frame = frame,
		.clear_color = colori(0, 0, 0, 255),
	});
}

