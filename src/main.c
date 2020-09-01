// wengwengweng

#include <string.h>
#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>

#include "app.h"
#include "fs.h"

typedef struct {
	lua_State* lua;
	int run_ref;
} l_ctx;

static l_ctx l;

static void check_arg(lua_State* L, int pos, int expected) {
	int actual = lua_type(L, pos);
	if (actual != expected) {
		luaL_argerror(L, pos, lua_pushfstring(L, "expected '%s', got '%s'", lua_typename(L, expected), lua_typename(L, actual)));
	}
}

static d_mouse str_to_d_mouse(const char* name) {
	if      (strcmp(name, "l") == 0) { return D_MOUSE_LEFT; }
	else if (strcmp(name, "r") == 0) { return D_MOUSE_RIGHT; }
	else if (strcmp(name, "m") == 0) { return D_MOUSE_MIDDLE; }
	else                             { return D_MOUSE_NONE; }
}

static d_key str_to_d_key(const char* name) {
	if      (strcmp(name, "q") == 0)     { return D_KEY_Q; }
	else if (strcmp(name, "w") == 0)     { return D_KEY_W; }
	else if (strcmp(name, "e") == 0)     { return D_KEY_E; }
	else if (strcmp(name, "r") == 0)     { return D_KEY_R; }
	else if (strcmp(name, "t") == 0)     { return D_KEY_T; }
	else if (strcmp(name, "y") == 0)     { return D_KEY_Y; }
	else if (strcmp(name, "u") == 0)     { return D_KEY_U; }
	else if (strcmp(name, "i") == 0)     { return D_KEY_I; }
	else if (strcmp(name, "o") == 0)     { return D_KEY_O; }
	else if (strcmp(name, "p") == 0)     { return D_KEY_P; }
	else if (strcmp(name, "a") == 0)     { return D_KEY_A; }
	else if (strcmp(name, "s") == 0)     { return D_KEY_S; }
	else if (strcmp(name, "d") == 0)     { return D_KEY_D; }
	else if (strcmp(name, "f") == 0)     { return D_KEY_F; }
	else if (strcmp(name, "g") == 0)     { return D_KEY_G; }
	else if (strcmp(name, "h") == 0)     { return D_KEY_H; }
	else if (strcmp(name, "j") == 0)     { return D_KEY_J; }
	else if (strcmp(name, "k") == 0)     { return D_KEY_K; }
	else if (strcmp(name, "l") == 0)     { return D_KEY_L; }
	else if (strcmp(name, "z") == 0)     { return D_KEY_Z; }
	else if (strcmp(name, "x") == 0)     { return D_KEY_X; }
	else if (strcmp(name, "c") == 0)     { return D_KEY_C; }
	else if (strcmp(name, "v") == 0)     { return D_KEY_V; }
	else if (strcmp(name, "b") == 0)     { return D_KEY_B; }
	else if (strcmp(name, "n") == 0)     { return D_KEY_N; }
	else if (strcmp(name, "m") == 0)     { return D_KEY_M; }
	else if (strcmp(name, "1") == 0)     { return D_KEY_1; }
	else if (strcmp(name, "2") == 0)     { return D_KEY_2; }
	else if (strcmp(name, "3") == 0)     { return D_KEY_3; }
	else if (strcmp(name, "4") == 0)     { return D_KEY_4; }
	else if (strcmp(name, "5") == 0)     { return D_KEY_5; }
	else if (strcmp(name, "6") == 0)     { return D_KEY_6; }
	else if (strcmp(name, "7") == 0)     { return D_KEY_7; }
	else if (strcmp(name, "8") == 0)     { return D_KEY_8; }
	else if (strcmp(name, "9") == 0)     { return D_KEY_9; }
	else if (strcmp(name, "0") == 0)     { return D_KEY_0; }
	else if (strcmp(name, "f1") == 0)    { return D_KEY_F1; }
	else if (strcmp(name, "f2") == 0)    { return D_KEY_F2; }
	else if (strcmp(name, "f3") == 0)    { return D_KEY_F3; }
	else if (strcmp(name, "f4") == 0)    { return D_KEY_F4; }
	else if (strcmp(name, "f5") == 0)    { return D_KEY_F5; }
	else if (strcmp(name, "f6") == 0)    { return D_KEY_F6; }
	else if (strcmp(name, "f7") == 0)    { return D_KEY_F7; }
	else if (strcmp(name, "f8") == 0)    { return D_KEY_F8; }
	else if (strcmp(name, "f9") == 0)    { return D_KEY_F9; }
	else if (strcmp(name, "f10") == 0)   { return D_KEY_F10; }
	else if (strcmp(name, "f11") == 0)   { return D_KEY_F11; }
	else if (strcmp(name, "f12") == 0)   { return D_KEY_F12; }
	else if (strcmp(name, "-") == 0)     { return D_KEY_MINUS; }
	else if (strcmp(name, "=") == 0)     { return D_KEY_EQUAL; }
	else if (strcmp(name, ",") == 0)     { return D_KEY_COMMA; }
	else if (strcmp(name, ".") == 0)     { return D_KEY_PERIOD; }
	else if (strcmp(name, "`") == 0)     { return D_KEY_BACKQUOTE; }
	else if (strcmp(name, "/") == 0)     { return D_KEY_SLASH; }
	else if (strcmp(name, "\\") == 0)    { return D_KEY_BACKSLASH; }
	else if (strcmp(name, ";") == 0)     { return D_KEY_SEMICOLON; }
	else if (strcmp(name, "'") == 0)     { return D_KEY_QUOTE; }
	else if (strcmp(name, "up") == 0)    { return D_KEY_UP; }
	else if (strcmp(name, "down") == 0)  { return D_KEY_DOWN; }
	else if (strcmp(name, "left") == 0)  { return D_KEY_LEFT; }
	else if (strcmp(name, "right") == 0) { return D_KEY_RIGHT; }
	else if (strcmp(name, "esc") == 0)   { return D_KEY_ESC; }
	else if (strcmp(name, "tab") == 0)   { return D_KEY_TAB; }
	else if (strcmp(name, " ") == 0)     { return D_KEY_SPACE; }
	else if (strcmp(name, "bksp") == 0)  { return D_KEY_BACKSPACE; }
	else if (strcmp(name, "enter") == 0) { return D_KEY_ENTER; }
	else if (strcmp(name, "[") == 0)     { return D_KEY_LBRACKET; }
	else if (strcmp(name, "]") == 0)     { return D_KEY_RBRACKET; }
	else if (strcmp(name, "lshft") == 0) { return D_KEY_LSHIFT; }
	else if (strcmp(name, "rshft") == 0) { return D_KEY_RSHIFT; }
	else if (strcmp(name, "lalt") == 0)  { return D_KEY_LALT; }
	else if (strcmp(name, "ralt") == 0)  { return D_KEY_RALT; }
	else if (strcmp(name, "lmeta") == 0) { return D_KEY_LMETA; }
	else if (strcmp(name, "rmeta") == 0) { return D_KEY_RMETA; }
	else if (strcmp(name, "lctrl") == 0) { return D_KEY_LCTRL; }
	else if (strcmp(name, "rctrl") == 0) { return D_KEY_RCTRL; }
	else                                 { return D_KEY_NONE; }
}

static int l_init(lua_State* L) {
	check_arg(L, 1, LUA_TSTRING);
	check_arg(L, 2, LUA_TNUMBER);
	check_arg(L, 3, LUA_TNUMBER);
	d_init(lua_tostring(L, 1), lua_tonumber(L, 2), lua_tonumber(L, 3));
	return 0;
}

static void l_run_inner() {
	lua_rawgeti(l.lua, LUA_REGISTRYINDEX, l.run_ref);
	if (lua_pcall(l.lua, 0, 0, 0) != LUA_OK) {
		d_quit();
		fprintf(stderr, "%s\n", lua_tostring(l.lua, -1));
	}
}

static int l_run(lua_State* L) {
	check_arg(L, 1, LUA_TFUNCTION);
	l.run_ref = luaL_ref(L, LUA_REGISTRYINDEX);
	d_run(l_run_inner);
	return 0;
}

static int l_quit(lua_State* L) {
	d_quit();
	return 0;
}

static int l_time(lua_State* L) {
	lua_pushnumber(L, d_time());
	return 1;
}

static int l_dt(lua_State* L) {
	lua_pushnumber(L, d_dt());
	return 1;
}

static int l_key_pressed(lua_State* L) {
	check_arg(L, 1, LUA_TSTRING);
	lua_pushboolean(L, d_key_pressed(str_to_d_key(lua_tostring(L, 1))));
	return 1;
}

static int l_key_released(lua_State* L) {
	check_arg(L, 1, LUA_TSTRING);
	lua_pushboolean(L, d_key_released(str_to_d_key(lua_tostring(L, 1))));
	return 1;
}

static int l_key_down(lua_State* L) {
	check_arg(L, 1, LUA_TSTRING);
	lua_pushboolean(L, d_key_down(str_to_d_key(lua_tostring(L, 1))));
	return 1;
}

static int l_mouse_pressed(lua_State* L) {
	check_arg(L, 1, LUA_TSTRING);
	lua_pushboolean(L, d_mouse_pressed(str_to_d_mouse(lua_tostring(L, 1))));
	return 1;
}

static int l_mouse_released(lua_State* L) {
	check_arg(L, 1, LUA_TSTRING);
	lua_pushboolean(L, d_mouse_released(str_to_d_mouse(lua_tostring(L, 1))));
	return 1;
}

static int l_mouse_down(lua_State* L) {
	check_arg(L, 1, LUA_TSTRING);
	lua_pushboolean(L, d_mouse_down(str_to_d_mouse(lua_tostring(L, 1))));
	return 1;
}

static int l_width(lua_State* L) {
	lua_pushnumber(L, d_width());
	return 1;
}

static int l_height(lua_State* L) {
	lua_pushnumber(L, d_height());
	return 1;
}

static int l_fread(lua_State* L) {
	check_arg(L, 1, LUA_TSTRING);
	lua_pushstring(L, d_fread(lua_tostring(L, 1)));
	return 1;
}

static int l_fexists(lua_State* L) {
	check_arg(L, 1, LUA_TSTRING);
	lua_pushboolean(L, d_fexists(lua_tostring(L, 1)));
	return 1;
}

static int l_clear(lua_State* L) {
	d_clear();
	return 0;
}

int run(const char* path) {

	lua_State* L = luaL_newstate();
	l.lua = L;
	luaL_openlibs(L);

	luaL_Reg reg[] = {
		{ "d_init", l_init, },
		{ "d_run", l_run, },
		{ "d_quit", l_quit, },
		{ "d_time", l_time, },
		{ "d_dt", l_dt, },
		{ "d_width", l_width, },
		{ "d_height", l_height, },
		{ "d_key_pressed", l_key_pressed, },
		{ "d_key_released", l_key_released, },
		{ "d_key_down", l_key_down, },
		{ "d_mouse_pressed", l_mouse_pressed, },
		{ "d_mouse_released", l_mouse_released, },
		{ "d_mouse_down", l_mouse_down, },
		{ "d_fread", l_fread, },
		{ "d_fexists", l_fexists, },
		{ "d_clear", l_clear, },
		{ NULL, NULL, }
	};

	for (int i = 0; reg[i].name != NULL; i++) {
		lua_pushcfunction(L, reg[i].func);
		lua_setglobal(L, reg[i].name);
	}

	if (luaL_dofile(L, path) != LUA_OK) {
		fprintf(stderr, "%s\n", lua_tostring(L, -1));
	}

	luaL_unref(L, LUA_REGISTRYINDEX, l.run_ref);
	lua_close(L);

	return 0;

}

int main(int argc, char** argv) {

	if (argc <= 1) {

		if (d_fexists("main.lua")) {
			return run("main.lua");
		} else {
			fprintf(stderr, "no\n");
		}

	} else {

		const char* action = argv[1];

		if (d_fexists(action)) {
			return run(action);
		} else {
			fprintf(stderr, "no\n");
		}

	}

	return 0;

}

