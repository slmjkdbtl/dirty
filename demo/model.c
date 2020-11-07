// wengwengweng

#include <dirty/dirty.h>

d_model model;
vec2 rot;

void init() {
	model = d_load_model("res/btfly.glb");
}

void frame() {

	if (d_key_pressed(D_KEY_ESC)) {
		d_quit();
	}

	if (d_mouse_down(D_MOUSE_LEFT)) {
		vec2 dpos = d_mouse_dpos();
		rot = vec2_add(rot, dpos);
	}

	d_scale(vec3f(480.0, 480.0, 480.0));
	d_rot_y(radf(rot.x));
	d_rot_x(radf(rot.y));
	d_draw_model(&model);

}

int main() {
	d_run((d_desc) {
		.title = "model",
		.init = init,
		.frame = frame,
	});
}

