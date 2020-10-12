// wengwengweng

#include <lua/lauxlib.h>
#include <dirty/dirty.h>

#include "utils.h"

static int l_read_text(lua_State *L) {
	const char *path = luaL_checkstring(L, 1);
	char *content = d_read_text(path);
	lua_pushstring(L, content);
	free(content);
	return 1;
}

static int l_is_file(lua_State *L) {
	const char *path = luaL_checkstring(L, 1);
	lua_pushboolean(L, d_is_file(path));
	return 1;
}

void l_fs_init(lua_State *L) {

	luaL_regfuncs(L, (luaL_Reg[]) {
		{ "d_read_text", l_read_text, },
		{ "d_is_file", l_is_file, },
		{ NULL, NULL },
	});

}

