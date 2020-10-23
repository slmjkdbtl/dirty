// wengwengweng

#include <lua/lauxlib.h>

#include "../res/extstd.lua.h"

void l_extstd_init(lua_State *L) {
	extstd_lua[extstd_lua_len] = '\0';
	luaL_dostring(L, (const char*)extstd_lua);
}

