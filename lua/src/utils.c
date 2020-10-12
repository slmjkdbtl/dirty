// wengwengweng

#include <sys/stat.h>
#include <stdbool.h>
#include <string.h>

#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>

#ifdef __APPLE__
#import <Foundation/Foundation.h>
#endif

#include "utils.h"

bool luaL_checkboolean(lua_State *L, int pos) {
	luaL_argexpected(L, lua_type(L, pos) == LUA_TBOOLEAN, pos, lua_typename(L, LUA_TBOOLEAN));
	return lua_toboolean(L, pos);
}

void *luaL_optudata(lua_State *L, int pos, const char *type, void *def) {
	return lua_isnoneornil(L, pos) ? def : luaL_checkudata(L, pos, type);
}

void lua_import(lua_State *L, luaL_Reg *reg) {
	for (int i = 0; reg[i].name != NULL; i++) {
		lua_register(L, reg[i].name, reg[i].func);
	}
}

bool streq(const char *a, const char *b) {
	return strcmp(a, b) == 0;
}

int str_to_enum(const char *str, enum_map *map) {
	for (int i = 0; map[i].str != NULL; i++) {
		if (streq(map[i].str, str)) {
			return map[i].val;
		}
	}
	return 0;
}

bool is_file(const char *path) {
	struct stat sb;
	return stat(path, &sb) == 0 && S_ISREG(sb.st_mode);
}

bool is_dir(const char *path) {
	struct stat sb;
	return stat(path, &sb) == 0 && S_ISDIR(sb.st_mode);
}

void to_dir(char *path) {
	int len = strlen(path);
	for (int i = len - 2; i >= 0; i--) {
		if (path[i] == '/') {
			path[i + 1] = '\0';
			return;
		}
	}
	path[0] = '\0';
	return;
}

void *get_res_dir(char *buf) {
#ifdef __APPLE__
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	const char* res_path = [[[NSBundle mainBundle] resourcePath] UTF8String];
	strcpy(buf, res_path);
	[pool drain];
#endif
}

