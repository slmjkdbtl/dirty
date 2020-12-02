// wengwengweng

#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>

#include "utils.h"

void l_app_init(lua_State *L, const char *path);
void l_app_quit(lua_State *L);
void l_gfx_init(lua_State *L);
void l_audio_init(lua_State *L);
void l_math_init(lua_State *L);
void l_fs_init(lua_State *L);
void l_json_init(lua_State *L);
void l_extstd_init(lua_State *L);

void d_run_lua(const char *path) {

	char dir[PATH_MAX];
	char load_path[PATH_MAX];
	char load_cpath[PATH_MAX];

	strcpy(dir, path);
	to_dir(dir);
	sprintf(load_path, "%s%s", dir, "?.lua");
	sprintf(load_cpath, "%s%s", dir, "?.so");

	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	l_app_init(L, dir);
	l_gfx_init(L);
	l_audio_init(L);
	l_math_init(L);
	l_fs_init(L);
	l_json_init(L);
	l_extstd_init(L);

	lua_getglobal(L, "package");
	lua_pushstring(L, load_path);
	lua_setfield(L, -2, "path");
	lua_pushstring(L, load_cpath);
	lua_setfield(L, -2, "cpath");
	lua_pop(L, 1);

	if (luaL_dofile(L, path) != LUA_OK) {
		fprintf(stderr, "%s\n", lua_tostring(L, -1));
	}

	l_app_quit(L);
	lua_close(L);

}


