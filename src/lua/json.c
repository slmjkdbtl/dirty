// wengwengweng

#include <lua/lauxlib.h>

#include "../res/json.lua.h"

void l_json_init(lua_State *L) {
	json_lua[json_lua_len] = '\0';
	luaL_dostring(L, (const char*)json_lua);
	lua_getfield(L, 1, "decode");
	lua_setglobal(L, "d_json_decode");
	lua_getfield(L, 1, "encode");
	lua_setglobal(L, "d_json_encode");
}

