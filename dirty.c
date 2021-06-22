#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

#define D_IMPL
#define D_CPU
// #define DT_VM_LOG
#include <d_plat.h>
#include <d_math.h>
#include <d_app.h>
#include <d_gfx.h>
#include <d_lang.h>

void init() {
	d_gfx_init((d_gfx_desc) {
		.width = 240,
		.height = 240,
		.clear_color = colori(0, 0, 0, 255),
	});
}

void frame() {

	if (d_app_key_pressed(D_KEY_ESC)) {
		d_app_quit();
	}

	d_blit_text("hi", vec2f(0, 0), colori(255, 255, 255, 255));
	d_gfx_present();

}

static dt_val dt_f_app_run(dt_vm *vm, int nargs) {
	d_app_run((d_app_desc) {
		.title = "hi",
		.init = init,
		.frame = frame,
		.width = 240,
		.height = 240,
	});
	return dt_nil;
}

int main(int argc, char **argv) {
	dt_map env = dt_map_new();
	dt_load_std(&env);
	dt_map_set_cfunc(&env, "app_run", dt_f_app_run);
	if (argc >= 2) {
		dt_dofile(&env, argv[1]);
	}
	return 0;
}
