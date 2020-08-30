// wengwengweng

#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>

#include "d/app.h"
#include "d/fs.h"

typedef struct {
	lua_State* lua;
	int run_ref;
} l_ctx;

static l_ctx l;

static int l_init(lua_State *L) {
	d_init("", 640, 480);
	return 0;
}

void l_run_inner() {
	lua_rawgeti(l.lua, LUA_REGISTRYINDEX, l.run_ref);
	lua_pcall(l.lua, 0, 0, 0);
}

static int l_run(lua_State *L) {
	if (lua_isfunction(L, -1)) {
		l.run_ref = luaL_ref(L, LUA_REGISTRYINDEX);
		d_run(l_run_inner);
	}
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

void run(const char* code) {

	l.lua = luaL_newstate();
	luaL_openlibs(l.lua);

	lua_pushcfunction(l.lua, l_init);
	lua_setglobal(l.lua, "d_init");

	lua_pushcfunction(l.lua, l_run);
	lua_setglobal(l.lua, "d_run");

	lua_pushcfunction(l.lua, l_time);
	lua_setglobal(l.lua, "d_time");

	lua_pushcfunction(l.lua, l_dt);
	lua_setglobal(l.lua, "d_dt");

	int r = luaL_dostring(l.lua, code);

	if (r != LUA_OK) {
		const char* err_msg = lua_tostring(l.lua, -1);
		printf("%s\n", err_msg);
	}

	luaL_unref(l.lua, LUA_REGISTRYINDEX, l.run_ref);
	lua_close(l.lua);

}

int main(int argc, char* argv[]) {

	if (argc <= 1) {

		if (d_fexists("main.lua")) {
			run(d_fread("main.lua"));
		} else {
			printf("no\n");
		}

	} else {

		const char* action = argv[1];

		if (d_fexists(action)) {
			run(d_fread(action));
		} else {
			printf("no\n");
		}

	}

}

