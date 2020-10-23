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

static int l_read_bytes(lua_State *L) {
	const char *path = luaL_checkstring(L, 1);
	size_t size;
	unsigned char *data = d_read_bytes(path, &size);
	unsigned char *lv = lua_newuserdata(L, size);
	luaL_setmetatable(L, "d_bytes");
	memcpy(lv, data, size);
	free(data);
	return 1;
}

static int l_is_file(lua_State *L) {
	const char *path = luaL_checkstring(L, 1);
	lua_pushboolean(L, d_is_file(path));
	return 1;
}

static int l_is_dir(lua_State *L) {
	const char *path = luaL_checkstring(L, 1);
	lua_pushboolean(L, d_is_dir(path));
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

static int l_data_read_text(lua_State *L) {
	const char *path = luaL_checkstring(L, 1);
	char *content = d_data_read_text(path);
	lua_pushstring(L, content);
	free(content);
	return 1;
}

static int l_data_read_bytes(lua_State *L) {
	const char *path = luaL_checkstring(L, 1);
	size_t size;
	unsigned char *data = d_data_read_bytes(path, &size);
	unsigned char *lv = lua_newuserdata(L, size);
	luaL_setmetatable(L, "d_bytes");
	memcpy(lv, data, size);
	free(data);
	return 1;
}

static int l_data_write_text(lua_State *L) {
	const char *path = luaL_checkstring(L, 1);
	const char *content = luaL_checkstring(L, 2);
	d_data_write_text(path, content);
	return 0;
}

static int l_data_is_file(lua_State *L) {
	const char *path = luaL_checkstring(L, 1);
	lua_pushboolean(L, d_data_is_file(path));
	return 1;
}

static int l_data_is_dir(lua_State *L) {
	const char *path = luaL_checkstring(L, 1);
	lua_pushboolean(L, d_data_is_dir(path));
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
		{ "d_read_bytes", l_read_bytes, },
		{ "d_read_dir", l_read_dir, },
		{ "d_is_file", l_is_file, },
		{ "d_is_dir", l_is_dir, },
		{ "d_data_read_text", l_data_read_text, },
		{ "d_data_read_bytes", l_data_read_bytes, },
		{ "d_data_write_text", l_data_write_text, },
		{ "d_data_is_file", l_data_is_file, },
		{ "d_data_is_dir", l_data_is_dir, },
		{ "d_extname", l_extname, },
		{ "d_basename", l_basename, },
		{ NULL, NULL, },
	});

}

