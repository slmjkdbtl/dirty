// wengwengweng

#include <lua/lauxlib.h>
#include <dirty/dirty.h>

#include "utils.h"

static int l_load_sound(lua_State *L) {
	const char *path = luaL_checkstring(L, 1);
	d_sound s = d_load_sound(path);
	lua_pushudata(L, d_sound, &s);
	return 1;
}

static int l_free_sound(lua_State *L) {
	d_sound *s = luaL_checkudata(L, 1, "d_sound");
	d_free_sound(s);
	return 0;
}

static int l_sound__index(lua_State *L) {

	d_sound *s = luaL_checkudata(L, 1, "d_sound");
	const char *arg = luaL_checkstring(L, 2);

	if (streq(arg, "len")) {
		lua_pushnumber(L, d_sound_len(s));
		return 1;
	}

	if (streq(arg, "free")) {
		lua_pushcfunction(L, l_free_sound);
		return 1;
	}

	return 0;

}

static int l_play(lua_State *L) {
	d_sound *s = luaL_checkudata(L, 1, "d_sound");
	d_playback *p = d_play(s);
	lua_pushlightuserdata(L, p);
	luaL_setmetatable(L, "d_playback");
	return 1;
}

static int l_playback__index(lua_State *L) {

	d_playback *p = luaL_checkudata(L, 1, "d_playback");
	const char *arg = luaL_checkstring(L, 2);

	if (streq(arg, "time")) {
		lua_pushnumber(L, d_playback_time(p));
		return 1;
	}

	if (streq(arg, "paused")) {
		lua_pushboolean(L, p->paused);
		return 1;
	}

	return 0;

}

static int l_playback__newindex(lua_State *L) {

	d_playback *p = luaL_checkudata(L, 1, "d_playback");
	const char *arg = luaL_checkstring(L, 2);

	if (streq(arg, "paused")) {
		p->paused = luaL_checkboolean(L, 3);
		return 0;
	}

	return 0;

}

void l_audio_init(lua_State *L) {

	luaL_regfuncs(L, (luaL_Reg[]) {
		{ "d_load_sound", l_load_sound, },
		{ "d_play", l_play, },
		{ NULL, NULL },
	});

	luaL_newmetatable(L, "d_sound");
	lua_pushcfunction(L, l_sound__index);
	lua_setfield(L, -2, "__index");

	luaL_newmetatable(L, "d_playback");
	lua_pushcfunction(L, l_playback__index);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, l_playback__newindex);
	lua_setfield(L, -2, "__newindex");

}

