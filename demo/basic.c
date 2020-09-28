// wengwengweng

#include <dirty/dirty.h>

void frame() {
	d_draw_text("oh hi mark", 24.0, 0.0, D_CENTER, WHITE);
}

int main() {
	d_run((d_desc) {
		.title = "basic",
		.frame = frame,
	});
}

