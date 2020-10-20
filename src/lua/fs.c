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

static int l_read_dir(lua_State *L) {
	const char *path = luaL_checkstring(L, 1);
	char **list = d_read_dir(path);
	lua_newtable(L);
	for (int i = 0; list[i] != NULL; i++) {
		lua_pushinteger(L, i + 1);
		lua_pushstring(L, list[i]);
		lua_settable(L, 2);
	}
	d_free_dir(list);
	return 1;
}

static int l_extname(lua_State *L) {
	const char *path = luaL_checkstring(L, 1);
	char *name = d_extname(path);
	lua_pushstring(L, name);
	if (name) {
		free(name);
	}
	return 1;
}

static int l_basename(lua_State *L) {
	const char *path = luaL_checkstring(L, 1);
	char *name = d_basename(path);
	lua_pushstring(L, name);
	if (name) {
		free(name);
	}
	return 1;
}

void l_fs_init(lua_State *L) {

	luaL_regfuncs(L, (luaL_Reg[]) {
		{ "d_read_text", l_read_text, },
		{ "d_is_file", l_is_file, },
		{ "d_read_dir", l_read_dir, },
		{ "d_extname", l_extname, },
		{ "d_basename", l_basename, },
		{ NULL, NULL },
	});

}

