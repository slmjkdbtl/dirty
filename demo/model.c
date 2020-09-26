// wengwengweng

#include <dirty/dirty.h>

d_model model;

static void frame() {

	if (d_key_pressed(D_KEY_ESC)) {
		d_quit();
	}

}

int main() {
	d_init("geom", 640, 480);
	model = d_load_model("res/btfly.glb");
	d_run(frame);
}

