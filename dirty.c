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

dt_vm *g_vm;
dt_func *app_frame;
dt_func *app_init;

void init() {
	if (app_init) {
		dt_vm_call_0(g_vm, app_init);
	}
}

void frame() {
	if (app_frame) {
		dt_vm_call_0(g_vm, app_frame);
	}
}

dt_val dt_f_app_run(dt_vm *vm, int nargs) {
	g_vm = vm;
	d_app_desc desc = (d_app_desc) {
		.title = "",
		.init = init,
		.frame = frame,
		.width = 480,
		.height = 480,
	};
	if (nargs >= 1) {
		dt_map *conf = dt_vm_get_map(vm, 0);
		dt_val frame = dt_map_get2(conf, "frame");
		if (dt_is_func(&frame)) {
			app_frame = frame.data.func;
		}
		dt_val init = dt_map_get2(conf, "init");
		if (dt_is_func(&init)) {
			app_init = init.data.func;
		}
		dt_val width = dt_map_get2(conf, "width");
		if (dt_is_num(&width)) {
			desc.width = width.data.num;
		}
		dt_val height = dt_map_get2(conf, "height");
		if (dt_is_num(&height)) {
			desc.height = height.data.num;
		}
	}
	d_app_run(desc);
	return dt_nil;
}

void load_app(dt_map *env) {
	dt_map *app = dt_map_new();
	dt_map_set_cfunc(app, "run", dt_f_app_run);
	dt_map_set_map(env, "app", app);
}

int main(int argc, char **argv) {
	dt_map *env = dt_map_new();
	dt_load_std(env);
	load_app(env);
	if (argc >= 2) {
		dt_dofile_ex(argv[1], env);
	}
	dt_map_free(env);
	return 0;
}
