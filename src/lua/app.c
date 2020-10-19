// wengwengweng

#include <lua/lauxlib.h>
#include <dirty/dirty.h>

#include "utils.h"

typedef struct {
	lua_State *lua;
	int init_ref;
	int frame_ref;
	int quit_ref;
	int err_ref;
	char path[PATH_MAX];
} l_app_ctx;

static l_app_ctx l_app;

static void l_init_inner() {
	lua_rawgeti(l_app.lua, LUA_REGISTRYINDEX, l_app.init_ref);
	if (lua_isfunction(l_app.lua, -1)) {
		lua_call(l_app.lua, 0, 0);
	}
}

static void l_frame_inner() {
	lua_rawgeti(l_app.lua, LUA_REGISTRYINDEX, l_app.frame_ref);
	if (lua_isfunction(l_app.lua, -1)) {
		lua_call(l_app.lua, 0, 0);
	}
}

static int l_run(lua_State *L) {

	luaL_checktable(L, 1);
	lua_getfield(L, 1, "init");
	l_app.init_ref = luaL_ref(L, LUA_REGISTRYINDEX);
	lua_getfield(L, 1, "frame");
	l_app.frame_ref = luaL_ref(L, LUA_REGISTRYINDEX);
	lua_getfield(L, 1, "title");

	// TODO: stack mess?
	d_run((d_desc) {
		.init = l_init_inner,
		.frame = l_frame_inner,
		.title = lua_getfield(L, 1, "title") ? luaL_checkstring(L, -1) : NULL,
		.width = lua_getfield(L, 1, "width") ? luaL_checkinteger(L, -1) : 0,
		.height = lua_getfield(L, 1, "height") ? luaL_checkinteger(L, -1) : 0,
		.vsync = lua_getfield(L, 1, "vsync") ? luaL_checkboolean(L, -1) : true,
		.path = l_app.path,
	});

	return 0;

}

static int l_quit(lua_State *L) {
	d_quit();
	return 0;
}

static int l_time(lua_State *L) {
	lua_pushnumber(L, d_time());
	return 1;
}

static int l_dt(lua_State *L) {
	lua_pushnumber(L, d_dt());
	return 1;
}

static int l_key_pressed(lua_State *L) {
	int k = luaL_checkinteger(L, 1);
	lua_pushboolean(L, d_key_pressed(k));
	return 1;
}

static int l_key_rpressed(lua_State *L) {
	int k = luaL_checkinteger(L, 1);
	lua_pushboolean(L, d_key_rpressed(k));
	return 1;
}

static int l_key_released(lua_State *L) {
	int k = luaL_checkinteger(L, 1);
	lua_pushboolean(L, d_key_released(k));
	return 1;
}

static int l_key_down(lua_State *L) {
	int k = luaL_checkinteger(L, 1);
	lua_pushboolean(L, d_key_down(k));
	return 1;
}

static int l_mouse_pressed(lua_State *L) {
	int m = luaL_checkinteger(L, 1);
	lua_pushboolean(L, d_mouse_pressed(m));
	return 1;
}

static int l_mouse_released(lua_State *L) {
	int m = luaL_checkinteger(L, 1);
	lua_pushboolean(L, d_mouse_released(m));
	return 1;
}

static int l_mouse_down(lua_State *L) {
	int m = luaL_checkinteger(L, 1);
	lua_pushboolean(L, d_mouse_down(m));
	return 1;
}

static int l_mouse_pos(lua_State *L) {
	vec2 mpos = d_mouse_pos();
	lua_pushudata(L, vec2, &mpos);
	return 1;
}

static int l_mouse_dpos(lua_State *L) {
	vec2 dpos = d_mouse_dpos();
	lua_pushudata(L, vec2, &dpos);
	return 1;
}

static int l_mouse_moved(lua_State *L) {
	lua_pushboolean(L, d_mouse_moved());
	return 1;
}

static int l_width(lua_State *L) {
	lua_pushnumber(L, d_width());
	return 1;
}

static int l_height(lua_State *L) {
	lua_pushnumber(L, d_height());
	return 1;
}

void l_app_init(lua_State *L, const char *path) {

	strcpy(l_app.path, path);
	l_app.lua = L;

	luaL_regfuncs(L, (luaL_Reg[]) {
		{ "d_run", l_run, },
		{ "d_quit", l_quit, },
		{ "d_time", l_time, },
		{ "d_dt", l_dt, },
		{ "d_width", l_width, },
		{ "d_height", l_height, },
		{ "d_key_pressed", l_key_pressed, },
		{ "d_key_rpressed", l_key_rpressed, },
		{ "d_key_released", l_key_released, },
		{ "d_key_down", l_key_down, },
		{ "d_mouse_pressed", l_mouse_pressed, },
		{ "d_mouse_released", l_mouse_released, },
		{ "d_mouse_down", l_mouse_down, },
		{ "d_mouse_pos", l_mouse_pos, },
		{ "d_mouse_dpos", l_mouse_dpos, },
		{ "d_mouse_moved", l_mouse_moved, },
		{ NULL, NULL },
	});

	luaL_regenum(L, "d_key", (luaL_Enum[]) {
		{ "q", D_KEY_Q, },
		{ "w", D_KEY_W, },
		{ "e", D_KEY_E, },
		{ "r", D_KEY_R, },
		{ "t", D_KEY_T, },
		{ "y", D_KEY_Y, },
		{ "u", D_KEY_U, },
		{ "i", D_KEY_I, },
		{ "o", D_KEY_O, },
		{ "p", D_KEY_P, },
		{ "a", D_KEY_A, },
		{ "s", D_KEY_S, },
		{ "d", D_KEY_D, },
		{ "f", D_KEY_F, },
		{ "g", D_KEY_G, },
		{ "h", D_KEY_H, },
		{ "j", D_KEY_J, },
		{ "k", D_KEY_K, },
		{ "l", D_KEY_L, },
		{ "z", D_KEY_Z, },
		{ "x", D_KEY_X, },
		{ "c", D_KEY_C, },
		{ "v", D_KEY_V, },
		{ "b", D_KEY_B, },
		{ "n", D_KEY_N, },
		{ "m", D_KEY_M, },
		{ "1", D_KEY_1, },
		{ "2", D_KEY_2, },
		{ "3", D_KEY_3, },
		{ "4", D_KEY_4, },
		{ "5", D_KEY_5, },
		{ "6", D_KEY_6, },
		{ "7", D_KEY_7, },
		{ "8", D_KEY_8, },
		{ "9", D_KEY_9, },
		{ "0", D_KEY_0, },
		{ "f1", D_KEY_F1, },
		{ "f2", D_KEY_F2, },
		{ "f3", D_KEY_F3, },
		{ "f4", D_KEY_F4, },
		{ "f5", D_KEY_F5, },
		{ "f6", D_KEY_F6, },
		{ "f7", D_KEY_F7, },
		{ "f8", D_KEY_F8, },
		{ "f9", D_KEY_F9, },
		{ "f10", D_KEY_F10, },
		{ "f11", D_KEY_F11, },
		{ "f12", D_KEY_F12, },
		{ "-", D_KEY_MINUS, },
		{ "=", D_KEY_EQUAL, },
		{ ",", D_KEY_COMMA, },
		{ ".", D_KEY_PERIOD, },
		{ "`", D_KEY_BACKQUOTE, },
		{ "/", D_KEY_SLASH, },
		{ "\\", D_KEY_BACKSLASH, },
		{ ";", D_KEY_SEMICOLON, },
		{ "'", D_KEY_QUOTE, },
		{ "up", D_KEY_UP, },
		{ "down", D_KEY_DOWN, },
		{ "left", D_KEY_LEFT, },
		{ "right", D_KEY_RIGHT, },
		{ "esc", D_KEY_ESC, },
		{ "tab", D_KEY_TAB, },
		{ " ", D_KEY_SPACE, },
		{ "space", D_KEY_SPACE, },
		{ "bksp", D_KEY_BACKSPACE, },
		{ "backspace", D_KEY_BACKSPACE, },
		{ "enter", D_KEY_ENTER, },
		{ "[", D_KEY_LBRACKET, },
		{ "]", D_KEY_RBRACKET, },
		{ "lshft", D_KEY_LSHIFT, },
		{ "rshft", D_KEY_RSHIFT, },
		{ "lalt", D_KEY_LALT, },
		{ "ralt", D_KEY_RALT, },
		{ "lmeta", D_KEY_LMETA, },
		{ "rmeta", D_KEY_RMETA, },
		{ "lctrl", D_KEY_LCTRL, },
		{ "rctrl", D_KEY_RCTRL, },
		{ NULL, 0, },
	});

	luaL_regenum(L, "d_mouse", (luaL_Enum[]) {
		{ "left", D_MOUSE_LEFT, },
		{ "right", D_MOUSE_RIGHT, },
		{ "middle", D_MOUSE_MIDDLE, },
		{ NULL, 0, },
	});

}

void l_app_quit(lua_State *L) {
	luaL_unref(L, LUA_REGISTRYINDEX, l_app.init_ref);
	luaL_unref(L, LUA_REGISTRYINDEX, l_app.frame_ref);
	luaL_unref(L, LUA_REGISTRYINDEX, l_app.quit_ref);
	luaL_unref(L, LUA_REGISTRYINDEX, l_app.err_ref);
}

