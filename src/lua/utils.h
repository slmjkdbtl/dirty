// wengwengweng

#ifndef LDIRTY_UTILS_H
#define LDIRTY_UTILS_H

#include <stdbool.h>
#include <string.h>

#define lua_pushudata(L, T, V) \
	T *lv = lua_newuserdata(L, sizeof(T)); \
	luaL_setmetatable(L, #T); \
	memcpy(lv, V, sizeof(T)); \

typedef struct {
	const char *str;
	int val;
} luaL_Enum;

bool luaL_args0(lua_State *L);
bool luaL_args1(lua_State *L, int type);
bool luaL_args2(lua_State *L, int t1, int t2);
bool luaL_args3(lua_State *L, int t1, int t2, int t3);
bool luaL_args4(lua_State *L, int t1, int t2, int t3, int t4);
void luaL_checktable(lua_State *L, int pos);
bool luaL_checkboolean(lua_State *L, int pos);
const char *lua_udatatype(lua_State *L, int pos);
void luaL_opttable(lua_State *L, int pos);
void *luaL_optudata(lua_State *L, int pos, const char *type, void *def);
void luaL_regfuncs(lua_State *L, luaL_Reg *reg);
void luaL_regenum(lua_State *L, const char *name, luaL_Enum *map);
void luaL_regtype(lua_State *L, const char *name, luaL_Reg *reg);
bool streq(const char *a, const char *b);
bool is_file(const char *path);
bool is_dir(const char *path);
void to_dir(char *path);

#endif

