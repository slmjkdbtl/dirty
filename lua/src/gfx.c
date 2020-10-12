// wengwengweng

#include <lua/lauxlib.h>
#include <dirty/dirty.h>

#include "utils.h"

static enum_map str_orig_map[] = {
	{ "topleft", D_TOP_LEFT },
	{ "top", D_TOP },
	{ "topright", D_TOP_RIGHT },
	{ "left", D_LEFT },
	{ "center", D_CENTER },
	{ "right", D_RIGHT },
	{ "botleft", D_BOT_LEFT },
	{ "bot", D_BOT },
	{ "botright", D_BOT_RIGHT },
	{ NULL, 0 },
};

static int l_clear(lua_State *L) {
	d_clear();
	return 0;
}

static int l_clear_color(lua_State *L) {
	d_clear_color();
	return 0;
}

static int l_clear_depth(lua_State *L) {
	d_clear_depth();
	return 0;
}

static int l_clear_stencil(lua_State *L) {
	d_clear_stencil();
	return 0;
}

static int l_depth_write(lua_State *L) {
	bool b = luaL_checkboolean(L, 1);
	d_depth_write(b);
	return 0;
}

static int l_depth_test(lua_State *L) {
	bool b = luaL_checkboolean(L, 1);
	d_depth_test(b);
	return 0;
}

static int l_stencil_write(lua_State *L) {
	bool b = luaL_checkboolean(L, 1);
	d_stencil_write(b);
	return 0;
}

static int l_stencil_test(lua_State *L) {
	bool b = luaL_checkboolean(L, 1);
	d_stencil_test(b);
	return 0;
}

static int l_push(lua_State *L) {
	d_push();
	return 0;
}

static int l_pop(lua_State *L) {
	d_pop();
	return 0;
}

static int l_move(lua_State *L) {
	vec3 *p = luaL_checkudata(L, 1, "vec3");
	d_move(*p);
	return 0;
}

static int l_move_xy(lua_State *L) {
	vec2 *p = luaL_checkudata(L, 1, "vec2");
	d_move_xy(*p);
	return 0;
}

static int l_scale(lua_State *L) {
	vec3 *p = luaL_checkudata(L, 1, "vec3");
	d_scale(*p);
	return 0;
}

static int l_scale_xy(lua_State *L) {
	vec2 *p = luaL_checkudata(L, 1, "vec2");
	d_scale_xy(*p);
	return 0;
}

static int l_rot_x(lua_State *L) {
	float a = luaL_checknumber(L, 1);
	d_rot_x(a);
	return 0;
}

static int l_rot_y(lua_State *L) {
	float a = luaL_checknumber(L, 1);
	d_rot_y(a);
	return 0;
}

static int l_rot_z(lua_State *L) {
	float a = luaL_checknumber(L, 1);
	d_rot_z(a);
	return 0;
}

static int l_send_f(lua_State *L) {
	const char *name = luaL_checkstring(L, 1);
	float val = luaL_checknumber(L, 2);
	d_send_f(name, val);
	return 0;
}

static int l_send_vec2(lua_State *L) {
	const char *name = luaL_checkstring(L, 1);
	vec2 *p = luaL_checkudata(L, 2, "vec2");
	d_send_vec2(name, *p);
	return 0;
}

static int l_send_vec3(lua_State *L) {
	const char *name = luaL_checkstring(L, 1);
	vec3 *p = luaL_checkudata(L, 2, "vec3");
	d_send_vec3(name, *p);
	return 0;
}

static int l_send_color(lua_State *L) {
	const char *name = luaL_checkstring(L, 1);
	color *c = luaL_checkudata(L, 2, "color");
	d_send_color(name, *c);
	return 0;
}

static int l_send_tex(lua_State *L) {
	const char *name = luaL_checkstring(L, 1);
	int slot = luaL_checknumber(L, 2);
	d_tex *t = luaL_checkudata(L, 3, "d_tex");
	d_send_tex(name, slot, t);
	return 0;
}

static int l_make_shader(lua_State *L) {
	const char *vs = luaL_checkstring(L, 1);
	const char *fs = luaL_checkstring(L, 2);
	d_shader p = d_make_shader(vs, fs);
	lua_pushudata(L, d_shader, &p);
	return 1;
}

static int l_free_shader(lua_State *L) {
	d_shader *s = luaL_checkudata(L, 1, "d_shader");
	d_free_shader(s);
	return 0;
}

static int l_shader__index(lua_State *L) {

	const char *arg = luaL_checkstring(L, 2);

	if (streq(arg, "free")) {
		lua_pushcfunction(L, l_free_shader);
		return 1;
	}

	return 0;

}

static int l_make_canvas(lua_State *L) {
	int w = luaL_checknumber(L, 1);
	int h = luaL_checknumber(L, 2);
	d_canvas c = d_make_canvas(w, h);
	lua_pushudata(L, d_canvas, &c);
	return 1;
}

static int l_free_canvas(lua_State *L) {
	d_canvas *c = luaL_checkudata(L, 1, "d_canvas");
	d_free_canvas(c);
	return 0;
}

static int l_canvas__index(lua_State *L) {

	d_canvas *c = luaL_checkudata(L, 1, "d_canvas");
	const char *arg = luaL_checkstring(L, 2);

	if (streq(arg, "width")) {
		lua_pushnumber(L, c->width);
		return 1;
	}

	if (streq(arg, "height")) {
		lua_pushnumber(L, c->height);
		return 1;
	}

	if (streq(arg, "free")) {
		lua_pushcfunction(L, l_free_canvas);
		return 1;
	}

	return 0;

}

static int l_load_tex(lua_State *L) {
	const char *path = luaL_checkstring(L, 1);
	d_tex t = d_load_tex(path);
	lua_pushudata(L, d_tex, &t);
	return 1;
}

static int l_free_tex(lua_State *L) {
	d_tex *t = luaL_checkudata(L, 1, "d_tex");
	d_free_tex(t);
	return 0;
}

static int l_tex__index(lua_State *L) {

	d_tex *t = luaL_checkudata(L, 1, "d_tex");
	const char *arg = luaL_checkstring(L, 2);

	if (streq(arg, "width")) {
		lua_pushnumber(L, t->width);
		return 1;
	}

	if (streq(arg, "height")) {
		lua_pushnumber(L, t->height);
		return 1;
	}

	if (streq(arg, "free")) {
		lua_pushcfunction(L, l_free_tex);
		return 1;
	}

	return 0;

}

static int l_use_shader(lua_State *L) {
	if (lua_isnoneornil(L, 1)) {
		d_use_shader(NULL);
	} else {
		d_shader *s = luaL_checkudata(L, 1, "d_shader");
		d_use_shader(s);
	}
	return 0;
}

static int l_use_canvas(lua_State *L) {
	if (lua_isnoneornil(L, 1)) {
		d_use_canvas(NULL);
	} else {
		d_canvas *c = luaL_checkudata(L, 1, "d_canvas");
		d_use_canvas(c);
	}
	return 0;
}

static int l_draw_tex(lua_State *L) {

	d_tex *t = luaL_checkudata(L, 1, "d_tex");
	quad q = lua_isnoneornil(L, 2) ? quadu() : *(quad*)luaL_checkudata(L, 2, "quad");
	color c = lua_isnoneornil(L, 3) ? coloru() : *(color*)luaL_checkudata(L, 3, "color");

	d_draw_tex(t, q, c);

	return 0;

}

static int l_draw_text(lua_State *L) {

	const char *str = luaL_checkstring(L, 1);
	float size = luaL_checknumber(L, 2);
	float wrap = luaL_optnumber(L, 3, 0.0);
	d_origin orig = str_to_enum(luaL_optstring(L, 4, "center"), str_orig_map);
	color c = lua_isnoneornil(L, 5) ? coloru() : *(color*)luaL_checkudata(L, 5, "color");

	d_draw_text(str, size, wrap, orig, c);

	return 0;

}

void l_gfx_init(lua_State *L) {

	luaL_Reg reg[] = {
		{ "d_clear", l_clear, },
		{ "d_clear_color", l_clear_color, },
		{ "d_clear_depth", l_clear_depth, },
		{ "d_clear_stencil", l_clear_stencil, },
		{ "d_depth_write", l_depth_write, },
		{ "d_depth_test", l_depth_test, },
		{ "d_stencil_write", l_stencil_write, },
		{ "d_stencil_test", l_stencil_test, },
		{ "d_push", l_push, },
		{ "d_pop", l_pop, },
		{ "d_move", l_move, },
		{ "d_move_xy", l_move_xy, },
		{ "d_scale", l_scale, },
		{ "d_scale_xy", l_scale_xy, },
		{ "d_rot_x", l_rot_x, },
		{ "d_rot_y", l_rot_y, },
		{ "d_rot_z", l_rot_z, },
		{ "d_send_f", l_send_f, },
		{ "d_send_vec2", l_send_vec2, },
		{ "d_send_vec3", l_send_vec3, },
		{ "d_send_color", l_send_color, },
		{ "d_send_tex", l_send_tex, },
		{ "d_make_shader", l_make_shader, },
		{ "d_make_canvas", l_make_canvas, },
		{ "d_load_tex", l_load_tex, },
		{ "d_use_shader", l_use_shader, },
		{ "d_use_canvas", l_use_canvas, },
		{ "d_draw_tex", l_draw_tex, },
		{ "d_draw_text", l_draw_text, },
		{ NULL, NULL },
	};

	lua_import(L, reg);

	luaL_newmetatable(L, "d_shader");
	lua_pushcfunction(L, l_shader__index);
	lua_setfield(L, -2, "__index");

	luaL_newmetatable(L, "d_canvas");
	lua_pushcfunction(L, l_canvas__index);
	lua_setfield(L, -2, "__index");

	luaL_newmetatable(L, "d_tex");
	lua_pushcfunction(L, l_tex__index);
	lua_setfield(L, -2, "__index");

}

