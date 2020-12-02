// wengwengweng

#include <unistd.h>

#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>

#include <dirty/dirty.h>

#include "utils.h"

// TODO: cleaner logic
int main(int argc, char **argv) {

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
			d_run_lua(path);
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
		d_run_lua(path);
		return EXIT_SUCCESS;
	}

	return EXIT_SUCCESS;

}

