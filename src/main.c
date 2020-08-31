// wengwengweng

#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>

#include <OpenGL/gl.h>
#include "d/app.h"
#include "d/fs.h"

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

int run(const char* code) {

	l.lua = luaL_newstate();
	luaL_openlibs(l.lua);

	lua_pushcfunction(l.lua, l_init);
	lua_setglobal(l.lua, "d_init");

	lua_pushcfunction(l.lua, l_run);
	lua_setglobal(l.lua, "d_run");

	lua_pushcfunction(l.lua, l_quit);
	lua_setglobal(l.lua, "d_quit");

	lua_pushcfunction(l.lua, l_time);
	lua_setglobal(l.lua, "d_time");

	lua_pushcfunction(l.lua, l_dt);
	lua_setglobal(l.lua, "d_dt");

	lua_pushcfunction(l.lua, l_key_pressed);
	lua_setglobal(l.lua, "d_key_pressed");

	lua_pushcfunction(l.lua, l_key_released);
	lua_setglobal(l.lua, "d_key_released");

	lua_pushcfunction(l.lua, l_key_down);
	lua_setglobal(l.lua, "d_key_down");

	lua_pushcfunction(l.lua, l_mouse_pressed);
	lua_setglobal(l.lua, "d_mouse_pressed");

	lua_pushcfunction(l.lua, l_mouse_released);
	lua_setglobal(l.lua, "d_mouse_released");

	lua_pushcfunction(l.lua, l_mouse_down);
	lua_setglobal(l.lua, "d_mouse_down");

	lua_pushcfunction(l.lua, l_width);
	lua_setglobal(l.lua, "d_width");

	lua_pushcfunction(l.lua, l_height);
	lua_setglobal(l.lua, "d_height");

	if (luaL_dostring(l.lua, code) != LUA_OK) {
		fprintf(stderr, "%s\n", lua_tostring(l.lua, -1));
	}

	luaL_unref(l.lua, LUA_REGISTRYINDEX, l.run_ref);
	lua_close(l.lua);

	return EXIT_SUCCESS;

}

static const char* vs_src =
"#version 120\n"
"attribute vec2 a_pos;"
"void main() {"
	"gl_Position = vec4(a_pos, 0.0, 1.0);"
"}"
;

static const char* fs_src =
"#version 120\n"
"void main() {"
	"gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0);"
"}"
;

int main(int argc, char* argv[]) {

	if (argc <= 1) {

		if (d_fexists("main.lua")) {
			return run(d_fread("main.lua"));
		} else {
			fprintf(stderr, "no\n");
		}

	} else {

		const char* action = argv[1];

		if (d_fexists(action)) {
			return run(d_fread(action));
		} else {
			fprintf(stderr, "no\n");
		}

	}

	return EXIT_SUCCESS;

}

