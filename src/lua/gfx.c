// wengwengweng

#include <lua/lauxlib.h>
#include <dirty/dirty.h>

#include "utils.h"

static int l_clear(lua_State *L) {
	d_clear();
	return 0;
}

static int l_make_img(lua_State *L) {
	int w = luaL_checkinteger(L, 1);
	int h = luaL_checkinteger(L, 2);
	d_img i = d_make_img(w, h);
	lua_pushudata(L, d_img, &i);
	return 1;
}

static int l_load_img(lua_State *L) {
	const char *path = luaL_checkstring(L, 1);
	d_img t = d_load_img(path);
	lua_pushudata(L, d_img, &t);
	return 1;
}

static int l_img_save(lua_State *L) {
	d_img *i = luaL_checkudata(L, 1, "d_img");
	const char *path = luaL_checkstring(L, 2);
	d_img_save(i, path);
	return 0;
}

static int l_free_img(lua_State *L) {
	d_img *i = luaL_checkudata(L, 1, "d_img");
	d_free_img(i);
	return 0;
}

static int l_img__index(lua_State *L) {

	d_img *i = luaL_checkudata(L, 1, "d_img");
	const char *arg = luaL_checkstring(L, 2);

	if (streq(arg, "width")) {
		lua_pushinteger(L, i->width);
		return 1;
	} if (streq(arg, "height")) {
		lua_pushinteger(L, i->height);
		return 1;
	} if (streq(arg, "free")) {
		lua_pushcfunction(L, l_free_img);
		return 1;
	} if (streq(arg, "save")) {
		lua_pushcfunction(L, l_img_save);
		return 1;
	} else {
		luaL_error(L, "unknown member '%s' of 'd_img'\n", arg);
	}

	return 0;

}

static int l_make_font(lua_State *L) {
	d_img *img = luaL_checkudata(L, 1, "d_img");
	int gw = luaL_checkinteger(L, 2);
	int gh = luaL_checkinteger(L, 3);
	const char *chars = luaL_optstring(L, 4, D_ASCII_CHARS);
	d_font f = d_make_font(*img, gw, gh, chars);
	lua_pushudata(L, d_font, &f);
	return 1;
}

static int l_free_font(lua_State *L) {
	d_font *f = luaL_checkudata(L, 1, "d_font");
	d_free_font(f);
	return 0;
}

static int l_font__index(lua_State *L) {

	d_font *f = luaL_checkudata(L, 1, "d_font");
	const char *arg = luaL_checkstring(L, 2);

	if (streq(arg, "free")) {
		lua_pushcfunction(L, l_free_font);
		return 1;
	} else if (streq(arg, "width")) {
		lua_pushinteger(L, f->width);
		return 1;
	} else if (streq(arg, "height")) {
		lua_pushinteger(L, f->height);
		return 1;
	} else {
		luaL_error(L, "unknown member '%s' of 'd_font'\n", arg);
	}

	return 0;

}

static int l_draw_img(lua_State *L) {

	d_img *i = luaL_checkudata(L, 1, "d_img");
	vec2 pos = lua_isnoneornil(L, 2) ? vec2u() : *(vec2*)luaL_checkudata(L, 2, "vec2");

	d_draw_img(i, pos);

	return 0;

}

static int l_draw_rect(lua_State *L) {

	vec2 *p1 = luaL_checkudata(L, 1, "vec2");
	vec2 *p2 = luaL_checkudata(L, 2, "vec2");
	color c = lua_isnoneornil(L, 3) ? coloru() : *(color*)luaL_checkudata(L, 3, "color");

	d_draw_rect(*p1, *p2, c);

	return 0;

}

static int l_draw_line(lua_State *L) {

	vec2 *p1 = luaL_checkudata(L, 1, "vec2");
	vec2 *p2 = luaL_checkudata(L, 2, "vec2");
	color c = lua_isnoneornil(L, 4) ? coloru() : *(color*)luaL_checkudata(L, 3, "color");

	d_draw_line(*p1, *p2, c);

	return 0;

}

static int l_draw_circle(lua_State *L) {

	vec2 *center = luaL_checkudata(L, 1, "vec2");
	float r = luaL_checknumber(L, 2);
	color c = lua_isnoneornil(L, 3) ? coloru() : *(color*)luaL_checkudata(L, 3, "color");

	d_draw_circle(*center, r, c);

	return 0;

}

void l_gfx_init(lua_State *L) {

	luaL_regfuncs(L, (luaL_Reg[]) {
		{ "d_clear", l_clear, },
		{ "d_make_img", l_make_img, },
		{ "d_load_img", l_load_img, },
		{ "d_make_font", l_make_font, },
		{ "d_draw_img", l_draw_img, },
		{ "d_draw_rect", l_draw_rect, },
		{ "d_draw_line", l_draw_line, },
		{ "d_draw_circle", l_draw_circle, },
		{ NULL, NULL, },
	});

	luaL_regenum(L, "d_origin", (luaL_Enum[]) {
		{ "top_left", D_TOP_LEFT, },
		{ "top", D_TOP, },
		{ "top_right", D_TOP_RIGHT, },
		{ "left", D_LEFT, },
		{ "center", D_CENTER, },
		{ "right", D_RIGHT, },
		{ "bot_left", D_BOT_LEFT, },
		{ "bot", D_BOT, },
		{ "bot_right", D_BOT_RIGHT, },
		{ NULL, 0, },
	});

	luaL_regtype(L, "d_img", (luaL_Reg[]) {
		{ "__index", l_img__index, },
		{ NULL, NULL, },
	});

	luaL_regtype(L, "d_font", (luaL_Reg[]) {
		{ "__index", l_font__index, },
		{ NULL, NULL, },
	});

}

