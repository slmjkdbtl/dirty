// wengwengweng

#include <stdbool.h>
#include <string.h>

#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>

#include "dirty.h"

#define pushudata(L, T, V) \
	T* lv = lua_newuserdata(L, sizeof(T)); \
	luaL_setmetatable(L, #T); \
	memcpy(lv, V, sizeof(T)); \

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
	const char* key = lua_tostring(L, 1);
	lua_pushboolean(L, d_mouse_down(str_to_d_mouse(key)));
	return 1;
}

static int l_mouse_pos(lua_State* L) {
	vec2 mpos = d_mouse_pos();
	pushudata(L, vec2, &mpos);
	return 1;
}

static int l_mouse_dpos(lua_State* L) {
	vec2 dpos = d_mouse_dpos();
	pushudata(L, vec2, &dpos);
	return 1;
}

static int l_mouse_moved(lua_State* L) {
	lua_pushboolean(L, d_mouse_moved());
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
	const char* content = d_fread(lua_tostring(L, 1), NULL);
	lua_pushstring(L, content);
	free((void*)content);
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

static int l_clear_color(lua_State* L) {
	d_clear_color();
	return 0;
}

static int l_clear_depth(lua_State* L) {
	d_clear_depth();
	return 0;
}

static int l_clear_stencil(lua_State* L) {
	d_clear_stencil();
	return 0;
}

static int l_depth_write(lua_State* L) {
	check_arg(L, 1, LUA_TBOOLEAN);
	d_depth_write(lua_tostring(L, 1));
	return 0;
}

static int l_depth_test(lua_State* L) {
	check_arg(L, 1, LUA_TBOOLEAN);
	d_depth_test(lua_tostring(L, 1));
	return 0;
}

static int l_stencil_write(lua_State* L) {
	check_arg(L, 1, LUA_TBOOLEAN);
	d_stencil_write(lua_tostring(L, 1));
	return 0;
}

static int l_stencil_test(lua_State* L) {
	check_arg(L, 1, LUA_TBOOLEAN);
	d_stencil_test(lua_tostring(L, 1));
	return 0;
}

static int l_push(lua_State* L) {
	d_push();
	return 0;
}

static int l_pop(lua_State* L) {
	d_pop();
	return 0;
}

static int l_move(lua_State* L) {
	check_arg(L, 1, LUA_TUSERDATA);
	vec3* p = luaL_checkudata(L, 1, "vec3");
	d_move(*p);
	return 0;
}

static int l_scale(lua_State* L) {
	check_arg(L, 1, LUA_TUSERDATA);
	vec3* p = luaL_checkudata(L, 1, "vec3");
	d_scale(*p);
	return 0;
}

static int l_rot_x(lua_State* L) {
	check_arg(L, 1, LUA_TNUMBER);
	d_rot_x(lua_tonumber(L, 1));
	return 0;
}

static int l_rot_y(lua_State* L) {
	check_arg(L, 1, LUA_TNUMBER);
	d_rot_y(lua_tonumber(L, 1));
	return 0;
}

static int l_rot_z(lua_State* L) {
	check_arg(L, 1, LUA_TNUMBER);
	d_rot_z(lua_tonumber(L, 1));
	return 0;
}

static int l_send_f(lua_State* L) {
	check_arg(L, 1, LUA_TSTRING);
	check_arg(L, 2, LUA_TNUMBER);
	d_send_f(lua_tostring(L, 1), lua_tonumber(L, 2));
	return 0;
}

static int l_send_vec2(lua_State* L) {
	check_arg(L, 1, LUA_TSTRING);
	check_arg(L, 2, LUA_TUSERDATA);
	vec2* p = luaL_checkudata(L, 2, "vec2");
	d_send_vec2(lua_tostring(L, 1), *p);
	return 0;
}

static int l_send_vec3(lua_State* L) {
	check_arg(L, 1, LUA_TSTRING);
	check_arg(L, 2, LUA_TUSERDATA);
	vec3* p = luaL_checkudata(L, 2, "vec3");
	d_send_vec3(lua_tostring(L, 1), *p);
	return 0;
}

static int l_send_color(lua_State* L) {
	check_arg(L, 1, LUA_TSTRING);
	check_arg(L, 2, LUA_TUSERDATA);
	color* c = (color*)luaL_checkudata(L, 2, "color");
	d_send_color(lua_tostring(L, 1), *c);
	return 0;
}

static int l_make_vec2(lua_State* L) {
	check_arg(L, 1, LUA_TNUMBER);
	check_arg(L, 2, LUA_TNUMBER);
	lua_Number x = lua_tonumber(L, 1);
	lua_Number y = lua_tonumber(L, 2);
	vec2 p = make_vec2(x, y);
	pushudata(L, vec2, &p);
	return 1;
}

static int l_vec2_len(lua_State* L) {
	check_arg(L, 1, LUA_TUSERDATA);
	vec2* p = luaL_checkudata(L, 1, "vec2");
	lua_pushnumber(L, vec2_len(*p));
	return 1;
}

static int l_vec2_unit(lua_State* L) {
	check_arg(L, 1, LUA_TUSERDATA);
	vec2* p = luaL_checkudata(L, 1, "vec2");
	vec2 unit = vec2_unit(*p);
	pushudata(L, vec2, &unit);
	return 1;
}

static int l_vec2_dist(lua_State* L) {
	check_arg(L, 1, LUA_TUSERDATA);
	check_arg(L, 2, LUA_TUSERDATA);
	vec2* p1 = luaL_checkudata(L, 1, "vec2");
	vec2* p2 = luaL_checkudata(L, 2, "vec2");
	lua_pushnumber(L, vec2_dist(*p1, *p2));
	return 1;
}

static int l_vec2_dot(lua_State* L) {
	check_arg(L, 1, LUA_TUSERDATA);
	check_arg(L, 2, LUA_TUSERDATA);
	vec2* p1 = luaL_checkudata(L, 1, "vec2");
	vec2* p2 = luaL_checkudata(L, 2, "vec2");
	lua_pushnumber(L, vec2_dot(*p1, *p2));
	return 1;
}

static int l_vec2__index(lua_State* L) {

	check_arg(L, 1, LUA_TUSERDATA);
	check_arg(L, 2, LUA_TSTRING);
	vec2* p = luaL_checkudata(L, 1, "vec2");
	const char* arg = lua_tostring(L, 2);

	if (strcmp(arg, "x") == 0) {
		lua_pushnumber(L, p->x);
		return 1;
	}

	if (strcmp(arg, "y") == 0) {
		lua_pushnumber(L, p->y);
		return 1;
	}

	if (strcmp(arg, "len") == 0) {
		lua_pushcfunction(L, l_vec2_len);
		return 1;
	}

	if (strcmp(arg, "unit") == 0) {
		lua_pushcfunction(L, l_vec2_unit);
		return 1;
	}

	if (strcmp(arg, "dist") == 0) {
		lua_pushcfunction(L, l_vec2_dist);
		return 1;
	}

	if (strcmp(arg, "dot") == 0) {
		lua_pushcfunction(L, l_vec2_dot);
		return 1;
	}

	return 0;

}

static int l_vec2__newindex(lua_State* L) {

	check_arg(L, 1, LUA_TUSERDATA);
	check_arg(L, 2, LUA_TSTRING);
	vec2* p = luaL_checkudata(L, 1, "vec2");
	const char* arg = lua_tostring(L, 2);

	if (strcmp(arg, "x") == 0) {
		check_arg(L, 3, LUA_TNUMBER);
		p->x = lua_tonumber(L, 3);
		return 0;
	}

	if (strcmp(arg, "y") == 0) {
		check_arg(L, 3, LUA_TNUMBER);
		p->y = lua_tonumber(L, 3);
		return 0;
	}

	return 0;

}

static int l_vec2__add(lua_State* L) {
	check_arg(L, 1, LUA_TUSERDATA);
	check_arg(L, 2, LUA_TUSERDATA);
	vec2* p1 = luaL_checkudata(L, 1, "vec2");
	vec2* p2 = luaL_checkudata(L, 2, "vec2");
	vec2 p = vec2_add(*p1, *p2);
	pushudata(L, vec2, &p);
	return 1;
}

static int l_vec2__sub(lua_State* L) {
	check_arg(L, 1, LUA_TUSERDATA);
	check_arg(L, 2, LUA_TUSERDATA);
	vec2* p1 = luaL_checkudata(L, 1, "vec2");
	vec2* p2 = luaL_checkudata(L, 2, "vec2");
	vec2 p = vec2_sub(*p1, *p2);
	pushudata(L, vec2, &p);
	return 1;
}

static int l_vec2__mul(lua_State* L) {
	check_arg(L, 1, LUA_TUSERDATA);
	check_arg(L, 2, LUA_TNUMBER);
	vec2* p = luaL_checkudata(L, 1, "vec2");
	float s = lua_tonumber(L, 2);
	vec2 sp = vec2_scale(*p, s);
	pushudata(L, vec2, &sp);
	return 1;
}

static int l_vec2__div(lua_State* L) {
	check_arg(L, 1, LUA_TUSERDATA);
	check_arg(L, 2, LUA_TNUMBER);
	vec2* p = luaL_checkudata(L, 1, "vec2");
	float s = lua_tonumber(L, 2);
	vec2 sp = vec2_scale(*p, 1.0 / s);
	pushudata(L, vec2, &sp);
	return 1;
}

static int l_vec2__eq(lua_State* L) {
	check_arg(L, 1, LUA_TUSERDATA);
	check_arg(L, 2, LUA_TUSERDATA);
	vec2* p1 = luaL_checkudata(L, 1, "vec2");
	vec2* p2 = luaL_checkudata(L, 2, "vec2");
	lua_pushboolean(L, vec2_eq(*p1, *p2));
	return 1;
}

static int l_make_vec3(lua_State* L) {
	check_arg(L, 1, LUA_TNUMBER);
	check_arg(L, 2, LUA_TNUMBER);
	check_arg(L, 3, LUA_TNUMBER);
	lua_Number x = lua_tonumber(L, 1);
	lua_Number y = lua_tonumber(L, 2);
	lua_Number z = lua_tonumber(L, 3);
	vec3 p = make_vec3(x, y, z);
	pushudata(L, vec3, &p);
	return 1;
}

static int l_vec3_len(lua_State* L) {
	check_arg(L, 1, LUA_TUSERDATA);
	vec3* p = luaL_checkudata(L, 1, "vec3");
	lua_pushnumber(L, vec3_len(*p));
	return 1;
}

static int l_vec3_unit(lua_State* L) {
	check_arg(L, 1, LUA_TUSERDATA);
	vec3* p = luaL_checkudata(L, 1, "vec3");
	vec3 unit = vec3_unit(*p);
	pushudata(L, vec3, &unit);
	return 1;
}

static int l_vec3_dist(lua_State* L) {
	check_arg(L, 1, LUA_TUSERDATA);
	check_arg(L, 2, LUA_TUSERDATA);
	vec3* p1 = luaL_checkudata(L, 1, "vec3");
	vec3* p2 = luaL_checkudata(L, 2, "vec3");
	lua_pushnumber(L, vec3_dist(*p1, *p2));
	return 1;
}

static int l_vec3_dot(lua_State* L) {
	check_arg(L, 1, LUA_TUSERDATA);
	check_arg(L, 2, LUA_TUSERDATA);
	vec3* p1 = (vec3*)luaL_checkudata(L, 1, "vec3");
	vec3* p2 = (vec3*)luaL_checkudata(L, 2, "vec3");
	lua_pushnumber(L, vec3_dot(*p1, *p2));
	return 1;
}

static int l_vec3__index(lua_State* L) {

	check_arg(L, 1, LUA_TUSERDATA);
	check_arg(L, 2, LUA_TSTRING);
	vec3* p = luaL_checkudata(L, 1, "vec3");
	const char* arg = lua_tostring(L, 2);

	if (strcmp(arg, "x") == 0) {
		lua_pushnumber(L, p->x);
		return 1;
	}

	if (strcmp(arg, "y") == 0) {
		lua_pushnumber(L, p->y);
		return 1;
	}

	if (strcmp(arg, "z") == 0) {
		lua_pushnumber(L, p->z);
		return 1;
	}

	if (strcmp(arg, "len") == 0) {
		lua_pushcfunction(L, l_vec3_len);
		return 1;
	}

	if (strcmp(arg, "unit") == 0) {
		lua_pushcfunction(L, l_vec3_unit);
		return 1;
	}

	if (strcmp(arg, "dist") == 0) {
		lua_pushcfunction(L, l_vec3_dist);
		return 1;
	}

	if (strcmp(arg, "dot") == 0) {
		lua_pushcfunction(L, l_vec3_dot);
		return 1;
	}

	return 0;

}

static int l_vec3__newindex(lua_State* L) {

	check_arg(L, 1, LUA_TUSERDATA);
	check_arg(L, 2, LUA_TSTRING);
	vec3* p = luaL_checkudata(L, 1, "vec3");
	const char* arg = lua_tostring(L, 2);

	if (strcmp(arg, "x") == 0) {
		check_arg(L, 3, LUA_TNUMBER);
		p->x = lua_tonumber(L, 3);
		return 0;
	}

	if (strcmp(arg, "y") == 0) {
		check_arg(L, 3, LUA_TNUMBER);
		p->y = lua_tonumber(L, 3);
		return 0;
	}

	if (strcmp(arg, "z") == 0) {
		check_arg(L, 3, LUA_TNUMBER);
		p->z = lua_tonumber(L, 3);
		return 0;
	}

	return 0;

}

static int l_vec3__add(lua_State* L) {
	check_arg(L, 1, LUA_TUSERDATA);
	check_arg(L, 2, LUA_TUSERDATA);
	vec3* p1 = luaL_checkudata(L, 1, "vec3");
	vec3* p2 = luaL_checkudata(L, 2, "vec3");
	vec3 p = vec3_add(*p1, *p2);
	pushudata(L, vec3, &p);
	return 1;
}

static int l_vec3__sub(lua_State* L) {
	check_arg(L, 1, LUA_TUSERDATA);
	check_arg(L, 2, LUA_TUSERDATA);
	vec3* p1 = luaL_checkudata(L, 1, "vec3");
	vec3* p2 = luaL_checkudata(L, 2, "vec3");
	vec3 p = vec3_sub(*p1, *p2);
	pushudata(L, vec3, &p);
	return 1;
}

static int l_vec3__mul(lua_State* L) {
	check_arg(L, 1, LUA_TUSERDATA);
	check_arg(L, 2, LUA_TNUMBER);
	vec3* p = luaL_checkudata(L, 1, "vec3");
	float s = lua_tonumber(L, 2);
	vec3 sp = vec3_scale(*p, s);
	pushudata(L, vec3, &sp);
	return 1;
}

static int l_vec3__div(lua_State* L) {
	check_arg(L, 1, LUA_TUSERDATA);
	check_arg(L, 2, LUA_TNUMBER);
	vec3* p = luaL_checkudata(L, 1, "vec3");
	float s = lua_tonumber(L, 2);
	vec3 sp = vec3_scale(*p, 1.0 / s);
	pushudata(L, vec3, &sp);
	return 1;
}

static int l_vec3__eq(lua_State* L) {
	check_arg(L, 1, LUA_TUSERDATA);
	check_arg(L, 2, LUA_TUSERDATA);
	vec3* p1 = luaL_checkudata(L, 1, "vec3");
	vec3* p2 = luaL_checkudata(L, 2, "vec3");
	lua_pushboolean(L, vec3_eq(*p1, *p2));
	return 1;
}

static int l_make_color(lua_State* L) {
	check_arg(L, 1, LUA_TNUMBER);
	check_arg(L, 2, LUA_TNUMBER);
	lua_Number r = lua_tonumber(L, 1);
	lua_Number g = lua_tonumber(L, 2);
	lua_Number b = lua_tonumber(L, 3);
	lua_Number a = lua_tonumber(L, 4);
	color c = make_color(r, g, b, a);
	pushudata(L, color, &c);
	return 1;
}

static int l_color__index(lua_State* L) {

	check_arg(L, 1, LUA_TUSERDATA);
	check_arg(L, 2, LUA_TSTRING);
	color* c = luaL_checkudata(L, 1, "color");
	const char* arg = lua_tostring(L, 2);

	if (strcmp(arg, "r") == 0) {
		lua_pushnumber(L, c->r);
		return 1;
	}

	if (strcmp(arg, "g") == 0) {
		lua_pushnumber(L, c->g);
		return 1;
	}

	if (strcmp(arg, "b") == 0) {
		lua_pushnumber(L, c->b);
		return 1;
	}

	if (strcmp(arg, "a") == 0) {
		lua_pushnumber(L, c->a);
		return 1;
	}

	return 0;

}

static int l_color__newindex(lua_State* L) {

	check_arg(L, 1, LUA_TUSERDATA);
	check_arg(L, 2, LUA_TSTRING);
	color* c = luaL_checkudata(L, 1, "color");
	const char* arg = lua_tostring(L, 2);

	if (strcmp(arg, "r") == 0) {
		check_arg(L, 3, LUA_TNUMBER);
		c->r = lua_tonumber(L, 3);
		return 0;
	}

	if (strcmp(arg, "g") == 0) {
		check_arg(L, 3, LUA_TNUMBER);
		c->g = lua_tonumber(L, 3);
		return 0;
	}

	if (strcmp(arg, "b") == 0) {
		check_arg(L, 3, LUA_TNUMBER);
		c->b = lua_tonumber(L, 3);
		return 0;
	}

	if (strcmp(arg, "a") == 0) {
		check_arg(L, 3, LUA_TNUMBER);
		c->a = lua_tonumber(L, 3);
		return 0;
	}

	return 0;

}

static int l_color__eq(lua_State* L) {
	check_arg(L, 1, LUA_TUSERDATA);
	check_arg(L, 2, LUA_TUSERDATA);
	color* c1 = luaL_checkudata(L, 1, "color");
	color* c2 = luaL_checkudata(L, 2, "color");
	lua_pushboolean(L, color_eq(*c1, *c2));
	return 1;
}

static int l_make_program(lua_State* L) {
	check_arg(L, 1, LUA_TSTRING);
	check_arg(L, 2, LUA_TSTRING);
	const char* vs = lua_tostring(L, 1);
	const char* fs = lua_tostring(L, 2);
	d_program p = d_make_program(vs, fs);
	pushudata(L, d_program, &p);
	return 1;
}

static int l_free_program(lua_State* L) {
	check_arg(L, 1, LUA_TUSERDATA);
	d_program* p = luaL_checkudata(L, 1, "d_program");
	d_free_program(p);
	return 0;
}

static int l_program__index(lua_State* L) {

	check_arg(L, 1, LUA_TUSERDATA);
	check_arg(L, 2, LUA_TSTRING);
	d_program* p = luaL_checkudata(L, 1, "d_program");
	const char* arg = lua_tostring(L, 2);

	if (strcmp(arg, "free") == 0) {
		lua_pushcfunction(L, l_free_program);
		return 1;
	}

	return 0;

}

int run(const char* path) {

	lua_State* L = luaL_newstate();
	l.lua = L;
	luaL_openlibs(L);

	luaL_Reg reg[] = {
		// app
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
		{ "d_mouse_pos", l_mouse_pos, },
		{ "d_mouse_dpos", l_mouse_dpos, },
		{ "d_mouse_moved", l_mouse_moved, },
		// gfx
		{ "d_clear", l_clear, },
		{ "d_clear_color", l_clear_color, },
		{ "d_clear_depth", l_clear_depth, },
		{ "d_clear_stencil", l_clear_stencil, },
		{ "d_depth_write", l_depth_write, },
		{ "d_depth_test", l_depth_test, },
		{ "d_stencil_write", l_stencil_write, },
		{ "d_stencil_test", l_stencil_test, },
		{ "d_push", l_push, },
		{ "d_pop", l_pop, },
		{ "d_move", l_move, },
		{ "d_scale", l_scale, },
		{ "d_rot_x", l_rot_x, },
		{ "d_rot_y", l_rot_y, },
		{ "d_rot_z", l_rot_z, },
		{ "d_send_f", l_send_f, },
		{ "d_send_vec2", l_send_vec2, },
		{ "d_send_vec3", l_send_vec3, },
		{ "d_send_color", l_send_color, },
		{ "d_make_program", l_make_program, },
		{ "d_free_program", l_free_program, },
		// audio
		// fs
		{ "d_fread", l_fread, },
		{ "d_fexists", l_fexists, },
		// math
		{ "vec2", l_make_vec2, },
		{ "vec3", l_make_vec3, },
		{ "color", l_make_color, },
		// end
		{ NULL, NULL, }
	};

	for (int i = 0; reg[i].name != NULL; i++) {
		lua_register(L, reg[i].name, reg[i].func);
	}

	luaL_newmetatable(L, "d_program");
	lua_pushcfunction(L, l_program__index);
	lua_setfield(L, -2, "__index");

	luaL_newmetatable(L, "vec2");
	lua_pushcfunction(L, l_vec2__index);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, l_vec2__newindex);
	lua_setfield(L, -2, "__newindex");
	lua_pushcfunction(L, l_vec2__add);
	lua_setfield(L, -2, "__add");
	lua_pushcfunction(L, l_vec2__sub);
	lua_setfield(L, -2, "__sub");
	lua_pushcfunction(L, l_vec2__mul);
	lua_setfield(L, -2, "__mul");
	lua_pushcfunction(L, l_vec2__div);
	lua_setfield(L, -2, "__div");
	lua_pushcfunction(L, l_vec2__eq);
	lua_setfield(L, -2, "__eq");

	luaL_newmetatable(L, "vec3");
	lua_pushcfunction(L, l_vec3__index);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, l_vec3__newindex);
	lua_setfield(L, -2, "__newindex");
	lua_pushcfunction(L, l_vec3__add);
	lua_setfield(L, -2, "__add");
	lua_pushcfunction(L, l_vec3__sub);
	lua_setfield(L, -2, "__sub");
	lua_pushcfunction(L, l_vec3__mul);
	lua_setfield(L, -2, "__mul");
	lua_pushcfunction(L, l_vec3__div);
	lua_setfield(L, -2, "__div");
	lua_pushcfunction(L, l_vec3__eq);
	lua_setfield(L, -2, "__eq");

	luaL_newmetatable(L, "color");
	lua_pushcfunction(L, l_color__index);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, l_color__newindex);
	lua_setfield(L, -2, "__newindex");
	lua_pushcfunction(L, l_color__eq);
	lua_setfield(L, -2, "__eq");

	if (luaL_dofile(L, path) != LUA_OK) {
		fprintf(stderr, "%s\n", lua_tostring(L, -1));
	}

	luaL_unref(L, LUA_REGISTRYINDEX, l.run_ref);
	lua_close(L);

	return 0;

}

int main(int argc, char** argv) {

	char* path = d_validate_path("main.lua");

	if (path) {

		return run(path);

	} else {

		if (argc >= 1) {

			const char* action = argv[1];

			if (d_fexists(action)) {
				return run(action);
			} else {
				fprintf(stderr, "no\n");
			}

		}

	}

	free(path);

	return 0;

}

