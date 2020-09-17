// wengwengweng

#include <dirty/dirty.h>

static void frame() {
	d_draw_text("oh hi mark", 24.0, 0.0, D_CENTER, WHITE);
}

int main() {
	d_init("hi", 640, 480);
	d_run(frame);
}

