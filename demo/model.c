// wengwengweng

#include <dirty/dirty.h>

d_model model;

void init() {
	model = d_load_model("res/btfly.glb");
}

void frame() {
	if (d_key_pressed(D_KEY_ESC)) {
		d_quit();
	}
	d_scale(vec3f(500.0, 500.0, 500.0));
// 	d_rot_y(d_time());
	d_draw_model(&model);
}

int main() {
	d_run((d_desc) {
		.title = "model",
		.init = init,
		.frame = frame,
	});
}

