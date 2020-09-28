// wengwengweng

#include <dirty/dirty.h>

d_model model;

void frame() {
	if (d_key_pressed(D_KEY_ESC)) {
		d_quit();
	}
}

int main() {
	d_run((d_desc) {
		.title = "model",
		.frame = frame,
	});
}

