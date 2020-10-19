// wengwengweng

#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>

#include <dirty/dirty.h>

#include "app.h"
#include "gfx.h"
#include "audio.h"
#include "math.h"
#include "fs.h"
#include "utils.h"

int run(const char *path) {

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

	return 0;

}

int main(int argc, char **argv) {

	// TODO: cleaner logic?

	char path[PATH_MAX];
	getcwd(path, sizeof(path));

	if (argc >= 2) {

		char *upath = argv[1];

		if (!streq(upath, ".")) {
			sprintf(path, "%s/%s", path, upath);
		}

		if (is_dir(path)) {
			sprintf(path, "%s/main.lua", path);
		}

		if (is_file(path)) {
			run(path);
			return EXIT_SUCCESS;
		}

	}

	getcwd(path, sizeof(path));
	sprintf(path, "%s/main.lua", path);

	if (!is_file(path)) {
		get_res_dir(path);
		sprintf(path, "%s/main.lua", path);
	}

	if (is_file(path)) {
		run(path);
		return EXIT_SUCCESS;
	}

	return EXIT_SUCCESS;

}

