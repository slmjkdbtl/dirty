#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

#define D_IMPL
#define D_CPU
// #define DT_VM_LOG
// #define DT_GC_LOG
// #define DT_GC_STRESS
#include <d_plat.h>
#include <d_math.h>
#include <d_app.h>
#include <d_gfx.h>
#include <d_lang.h>

dt_vm* g_vm;
dt_func* app_frame;
dt_func* app_init;

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

dt_val dt_f_app_run(dt_vm* vm, int nargs) {
	g_vm = vm;
	d_app_desc desc = (d_app_desc) {
		.title = "",
		.init = init,
		.frame = frame,
		.width = 480,
		.height = 480,
	};
	if (nargs >= 1) {
		dt_map* conf = dt_vm_get_map(vm, 0);
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

dt_val dt_f_app_quit(dt_vm* vm, int nargs) {
	d_app_quit();
	return dt_nil;
}

dt_val dt_f_app_width(dt_vm* vm, int nargs) {
	return dt_val_num(d_app_width());
}

dt_val dt_f_app_height(dt_vm* vm, int nargs) {
	return dt_val_num(d_app_height());
}

dt_val dt_f_app_time(dt_vm* vm, int nargs) {
	return dt_val_num(d_app_time());
}

dt_val dt_f_app_dt(dt_vm* vm, int nargs) {
	return dt_val_num(d_app_dt());
}

bool streq(char* s1, char* s2) {
	return strcmp(s1, s2) == 0;
}

d_key str_to_d_key(char* k) {

	if      (streq(k, "a")) return D_KEY_A;
	else if (streq(k, "b")) return D_KEY_B;
	else if (streq(k, "c")) return D_KEY_C;
	else if (streq(k, "d")) return D_KEY_D;
	else if (streq(k, "e")) return D_KEY_E;
	else if (streq(k, "f")) return D_KEY_F;
	else if (streq(k, "g")) return D_KEY_G;
	else if (streq(k, "h")) return D_KEY_H;
	else if (streq(k, "i")) return D_KEY_I;
	else if (streq(k, "j")) return D_KEY_J;
	else if (streq(k, "k")) return D_KEY_K;
	else if (streq(k, "l")) return D_KEY_L;
	else if (streq(k, "m")) return D_KEY_M;
	else if (streq(k, "n")) return D_KEY_N;
	else if (streq(k, "o")) return D_KEY_O;
	else if (streq(k, "p")) return D_KEY_P;
	else if (streq(k, "q")) return D_KEY_Q;
	else if (streq(k, "r")) return D_KEY_R;
	else if (streq(k, "s")) return D_KEY_S;
	else if (streq(k, "t")) return D_KEY_T;
	else if (streq(k, "u")) return D_KEY_U;
	else if (streq(k, "v")) return D_KEY_V;
	else if (streq(k, "w")) return D_KEY_W;
	else if (streq(k, "x")) return D_KEY_X;
	else if (streq(k, "y")) return D_KEY_Y;
	else if (streq(k, "z")) return D_KEY_Z;
	else if (streq(k, "1")) return D_KEY_1;
	else if (streq(k, "2")) return D_KEY_2;
	else if (streq(k, "3")) return D_KEY_3;
	else if (streq(k, "4")) return D_KEY_4;
	else if (streq(k, "5")) return D_KEY_5;
	else if (streq(k, "6")) return D_KEY_6;
	else if (streq(k, "7")) return D_KEY_7;
	else if (streq(k, "8")) return D_KEY_8;
	else if (streq(k, "9")) return D_KEY_9;
	else if (streq(k, "0")) return D_KEY_0;
	else if (streq(k, "-")) return D_KEY_MINUS;
	else if (streq(k, "=")) return D_KEY_EQUAL;
	else if (streq(k, " ")) return D_KEY_SPACE;
	else if (streq(k, ",")) return D_KEY_COMMA;
	else if (streq(k, ".")) return D_KEY_DOT;
	else if (streq(k, "/")) return D_KEY_SLASH;
	else if (streq(k, "[")) return D_KEY_LBRACKET;
	else if (streq(k, "]")) return D_KEY_RBRACKET;
	else if (streq(k, "\\")) return D_KEY_BACKSLASH;
	else if (streq(k, ";")) return D_KEY_SEMICOLON;
	else if (streq(k, "return")) return D_KEY_RETURN;
	else if (streq(k, "esc")) return D_KEY_ESC;
	else if (streq(k, "backspace")) return D_KEY_BACKSPACE;
	else if (streq(k, "tab")) return D_KEY_TAB;
	else if (streq(k, "'")) return D_KEY_QUOTE;
	else if (streq(k, "`")) return D_KEY_GRAVES;
	else if (streq(k, "f1")) return D_KEY_F1;
	else if (streq(k, "f2")) return D_KEY_F2;
	else if (streq(k, "f3")) return D_KEY_F3;
	else if (streq(k, "f4")) return D_KEY_F4;
	else if (streq(k, "f5")) return D_KEY_F5;
	else if (streq(k, "f6")) return D_KEY_F6;
	else if (streq(k, "f7")) return D_KEY_F7;
	else if (streq(k, "f5")) return D_KEY_F5;
	else if (streq(k, "f6")) return D_KEY_F6;
	else if (streq(k, "f7")) return D_KEY_F7;
	else if (streq(k, "f8")) return D_KEY_F8;
	else if (streq(k, "f9")) return D_KEY_F9;
	else if (streq(k, "f10")) return D_KEY_F10;
	else if (streq(k, "f11")) return D_KEY_F11;
	else if (streq(k, "f12")) return D_KEY_F12;
	else if (streq(k, "right")) return D_KEY_RIGHT;
	else if (streq(k, "left")) return D_KEY_LEFT;
	else if (streq(k, "down")) return D_KEY_DOWN;
	else if (streq(k, "up")) return D_KEY_UP;
	else if (streq(k, "lmeta")) return D_KEY_LMETA;
	else if (streq(k, "rmeta")) return D_KEY_RMETA;
	else if (streq(k, "lshift")) return D_KEY_LSHIFT;
	else if (streq(k, "rshift")) return D_KEY_RSHIFT;
	else if (streq(k, "lalt")) return D_KEY_LALT;
	else if (streq(k, "ralt")) return D_KEY_RALT;
	else if (streq(k, "lctrl")) return D_KEY_LCTRL;
	else if (streq(k, "rctrl")) return D_KEY_RCTRL;
	return D_KEY_NONE;
}

dt_val dt_f_app_key_pressed(dt_vm* vm, int nargs) {
	if (nargs == 0) {
		return dt_val_bool(false);
	}
	return dt_val_bool(d_app_key_pressed(str_to_d_key(dt_vm_get_cstr(vm, 0))));
}

dt_val dt_f_app_key_down(dt_vm* vm, int nargs) {
	if (nargs == 0) {
		return dt_val_bool(false);
	}
	return dt_val_bool(d_app_key_down(str_to_d_key(dt_vm_get_cstr(vm, 0))));
}

dt_val dt_f_app_key_released(dt_vm* vm, int nargs) {
	if (nargs == 0) {
		return dt_val_bool(false);
	}
	return dt_val_bool(d_app_key_released(str_to_d_key(dt_vm_get_cstr(vm, 0))));
}

void load_app(dt_vm* vm) {
	dt_map* app = dt_map_new(NULL);
	dt_map_cset_cfunc(vm, app, "run", dt_f_app_run);
	dt_map_cset_cfunc(vm, app, "quit", dt_f_app_quit);
	dt_map_cset_cfunc(vm, app, "width", dt_f_app_width);
	dt_map_cset_cfunc(vm, app, "height", dt_f_app_height);
	dt_map_cset_cfunc(vm, app, "time", dt_f_app_time);
	dt_map_cset_cfunc(vm, app, "dt", dt_f_app_dt);
	dt_map_cset_cfunc(vm, app, "key_pressed", dt_f_app_key_pressed);
	dt_map_cset_cfunc(vm, app, "key_down", dt_f_app_key_down);
	dt_map_cset_cfunc(vm, app, "key_released", dt_f_app_key_released);
	dt_map_cset_map(vm, vm->globals, "app", app);
}

void load_gfx(dt_vm* vm) {
	dt_map* gfx = dt_map_new(NULL);
	dt_map_cset_map(vm, vm->globals, "gfx", gfx);
}

int main(int argc, char** argv) {
	dt_vm vm = dt_vm_new();
	dt_load_std(&vm);
	load_app(&vm);
	load_gfx(&vm);
	if (argc >= 2) {
		dt_dofile(&vm, argv[1]);
	}
	return 0;
}
