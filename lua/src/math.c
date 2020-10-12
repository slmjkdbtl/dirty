// wengwengweng

#include <lua/lauxlib.h>
#include <dirty/dirty.h>

#include "utils.h"

static int l_vec2f(lua_State *L) {
	float x = luaL_optnumber(L, 1, 0.0);
	float y = luaL_optnumber(L, 2, x);
	vec2 p = vec2f(x, y);
	lua_pushudata(L, vec2, &p);
	return 1;
}

static int l_vec2_len(lua_State *L) {
	vec2 *p = luaL_checkudata(L, 1, "vec2");
	lua_pushnumber(L, vec2_len(*p));
	return 1;
}

static int l_vec2_unit(lua_State *L) {
	vec2 *p = luaL_checkudata(L, 1, "vec2");
	vec2 unit = vec2_unit(*p);
	lua_pushudata(L, vec2, &unit);
	return 1;
}

static int l_vec2_dist(lua_State *L) {
	vec2 *p1 = luaL_checkudata(L, 1, "vec2");
	vec2 *p2 = luaL_checkudata(L, 2, "vec2");
	lua_pushnumber(L, vec2_dist(*p1, *p2));
	return 1;
}

static int l_vec2_dot(lua_State *L) {
	vec2 *p1 = luaL_checkudata(L, 1, "vec2");
	vec2 *p2 = luaL_checkudata(L, 2, "vec2");
	lua_pushnumber(L, vec2_dot(*p1, *p2));
	return 1;
}

static int l_vec2__index(lua_State *L) {

	vec2 *p = luaL_checkudata(L, 1, "vec2");
	const char *arg = luaL_checkstring(L, 2);

	if (streq(arg, "x")) {
		lua_pushnumber(L, p->x);
		return 1;
	}

	if (streq(arg, "y")) {
		lua_pushnumber(L, p->y);
		return 1;
	}

	if (streq(arg, "len")) {
		lua_pushcfunction(L, l_vec2_len);
		return 1;
	}

	if (streq(arg, "unit")) {
		lua_pushcfunction(L, l_vec2_unit);
		return 1;
	}

	if (streq(arg, "dist")) {
		lua_pushcfunction(L, l_vec2_dist);
		return 1;
	}

	if (streq(arg, "dot")) {
		lua_pushcfunction(L, l_vec2_dot);
		return 1;
	}

	return 0;

}

static int l_vec2__newindex(lua_State *L) {

	vec2 *p = luaL_checkudata(L, 1, "vec2");
	const char *arg = luaL_checkstring(L, 2);

	if (streq(arg, "x")) {
		p->x = luaL_checknumber(L, 3);
		return 0;
	}

	if (streq(arg, "y")) {
		p->y = luaL_checknumber(L, 3);
		return 0;
	}

	return 0;

}

static int l_vec2__add(lua_State *L) {
	vec2 *p1 = luaL_checkudata(L, 1, "vec2");
	vec2 *p2 = luaL_checkudata(L, 2, "vec2");
	vec2 p = vec2_add(*p1, *p2);
	lua_pushudata(L, vec2, &p);
	return 1;
}

static int l_vec2__sub(lua_State *L) {
	vec2 *p1 = luaL_checkudata(L, 1, "vec2");
	vec2 *p2 = luaL_checkudata(L, 2, "vec2");
	vec2 p = vec2_sub(*p1, *p2);
	lua_pushudata(L, vec2, &p);
	return 1;
}

static int l_vec2__mul(lua_State *L) {
	vec2 *p = luaL_checkudata(L, 1, "vec2");
	float s = luaL_checknumber(L, 2);
	vec2 sp = vec2_scale(*p, s);
	lua_pushudata(L, vec2, &sp);
	return 1;
}

static int l_vec2__div(lua_State *L) {
	vec2 *p = luaL_checkudata(L, 1, "vec2");
	float s = luaL_checknumber(L, 2);
	vec2 sp = vec2_scale(*p, 1.0 / s);
	lua_pushudata(L, vec2, &sp);
	return 1;
}

static int l_vec2__eq(lua_State *L) {
	vec2 *p1 = luaL_checkudata(L, 1, "vec2");
	vec2 *p2 = luaL_checkudata(L, 2, "vec2");
	lua_pushboolean(L, vec2_eq(*p1, *p2));
	return 1;
}

static int l_vec3f(lua_State *L) {
	float x = luaL_optnumber(L, 1, 0.0);
	float y = luaL_optnumber(L, 2, x);
	float z = luaL_optnumber(L, 3, 0.0);
	vec3 p = vec3f(x, y, z);
	lua_pushudata(L, vec3, &p);
	return 1;
}

static int l_vec3_len(lua_State *L) {
	vec3 *p = luaL_checkudata(L, 1, "vec3");
	lua_pushnumber(L, vec3_len(*p));
	return 1;
}

static int l_vec3_unit(lua_State *L) {
	vec3 *p = luaL_checkudata(L, 1, "vec3");
	vec3 unit = vec3_unit(*p);
	lua_pushudata(L, vec3, &unit);
	return 1;
}

static int l_vec3_dist(lua_State *L) {
	vec3 *p1 = luaL_checkudata(L, 1, "vec3");
	vec3 *p2 = luaL_checkudata(L, 2, "vec3");
	lua_pushnumber(L, vec3_dist(*p1, *p2));
	return 1;
}

static int l_vec3_dot(lua_State *L) {
	vec3 *p1 = (vec3*)luaL_checkudata(L, 1, "vec3");
	vec3 *p2 = (vec3*)luaL_checkudata(L, 2, "vec3");
	lua_pushnumber(L, vec3_dot(*p1, *p2));
	return 1;
}

static int l_vec3__index(lua_State *L) {

	vec3 *p = luaL_checkudata(L, 1, "vec3");
	const char *arg = luaL_checkstring(L, 2);

	if (streq(arg, "x")) {
		lua_pushnumber(L, p->x);
		return 1;
	}

	if (streq(arg, "y")) {
		lua_pushnumber(L, p->y);
		return 1;
	}

	if (streq(arg, "z")) {
		lua_pushnumber(L, p->z);
		return 1;
	}

	if (streq(arg, "len")) {
		lua_pushcfunction(L, l_vec3_len);
		return 1;
	}

	if (streq(arg, "unit")) {
		lua_pushcfunction(L, l_vec3_unit);
		return 1;
	}

	if (streq(arg, "dist")) {
		lua_pushcfunction(L, l_vec3_dist);
		return 1;
	}

	if (streq(arg, "dot")) {
		lua_pushcfunction(L, l_vec3_dot);
		return 1;
	}

	return 0;

}

static int l_vec3__newindex(lua_State *L) {

	vec3 *p = luaL_checkudata(L, 1, "vec3");
	const char *arg = luaL_checkstring(L, 2);

	if (streq(arg, "x")) {
		p->x = luaL_checknumber(L, 3);
		return 0;
	}

	if (streq(arg, "y")) {
		p->y = luaL_checknumber(L, 3);
		return 0;
	}

	if (streq(arg, "z")) {
		p->z = luaL_checknumber(L, 3);
		return 0;
	}

	return 0;

}

static int l_vec3__add(lua_State *L) {
	vec3 *p1 = luaL_checkudata(L, 1, "vec3");
	vec3 *p2 = luaL_checkudata(L, 2, "vec3");
	vec3 p = vec3_add(*p1, *p2);
	lua_pushudata(L, vec3, &p);
	return 1;
}

static int l_vec3__sub(lua_State *L) {
	vec3 *p1 = luaL_checkudata(L, 1, "vec3");
	vec3 *p2 = luaL_checkudata(L, 2, "vec3");
	vec3 p = vec3_sub(*p1, *p2);
	lua_pushudata(L, vec3, &p);
	return 1;
}

static int l_vec3__mul(lua_State *L) {
	vec3 *p = luaL_checkudata(L, 1, "vec3");
	float s = luaL_checknumber(L, 2);
	vec3 sp = vec3_scale(*p, s);
	lua_pushudata(L, vec3, &sp);
	return 1;
}

static int l_vec3__div(lua_State *L) {
	vec3 *p = luaL_checkudata(L, 1, "vec3");
	float s = luaL_checknumber(L, 2);
	vec3 sp = vec3_scale(*p, 1.0 / s);
	lua_pushudata(L, vec3, &sp);
	return 1;
}

static int l_vec3__eq(lua_State *L) {
	vec3 *p1 = luaL_checkudata(L, 1, "vec3");
	vec3 *p2 = luaL_checkudata(L, 2, "vec3");
	lua_pushboolean(L, vec3_eq(*p1, *p2));
	return 1;
}

static int l_colorf(lua_State *L) {
	float r = luaL_checknumber(L, 1);
	float g = luaL_checknumber(L, 2);
	float b = luaL_checknumber(L, 3);
	float a = luaL_checknumber(L, 4);
	color c = colorf(r, g, b, a);
	lua_pushudata(L, color, &c);
	return 1;
}

static int l_color__index(lua_State *L) {

	color *c = luaL_checkudata(L, 1, "color");
	const char *arg = luaL_checkstring(L, 2);

	if (streq(arg, "r")) {
		lua_pushnumber(L, c->r);
		return 1;
	}

	if (streq(arg, "g")) {
		lua_pushnumber(L, c->g);
		return 1;
	}

	if (streq(arg, "b")) {
		lua_pushnumber(L, c->b);
		return 1;
	}

	if (streq(arg, "a")) {
		lua_pushnumber(L, c->a);
		return 1;
	}

	return 0;

}

static int l_color__newindex(lua_State *L) {

	color *c = luaL_checkudata(L, 1, "color");
	const char *arg = luaL_checkstring(L, 2);

	if (streq(arg, "r")) {
		c->r = luaL_checknumber(L, 3);
		return 0;
	}

	if (streq(arg, "g")) {
		c->g = luaL_checknumber(L, 3);
		return 0;
	}

	if (streq(arg, "b")) {
		c->b = luaL_checknumber(L, 3);
		return 0;
	}

	if (streq(arg, "a")) {
		c->a = luaL_checknumber(L, 3);
		return 0;
	}

	return 0;

}

static int l_color__eq(lua_State *L) {
	color *c1 = luaL_checkudata(L, 1, "color");
	color *c2 = luaL_checkudata(L, 2, "color");
	lua_pushboolean(L, color_eq(*c1, *c2));
	return 1;
}

static int l_quadf(lua_State *L) {
	float x = luaL_checknumber(L, 1);
	float y = luaL_checknumber(L, 2);
	float w = luaL_checknumber(L, 3);
	float h = luaL_checknumber(L, 4);
	quad q = quadf(x, y, w, h);
	lua_pushudata(L, quad, &q);
	return 1;
}

static int l_quad__index(lua_State *L) {

	quad *q = luaL_checkudata(L, 1, "quad");
	const char *arg = luaL_checkstring(L, 2);

	if (streq(arg, "x")) {
		lua_pushnumber(L, q->x);
		return 1;
	}

	if (streq(arg, "y")) {
		lua_pushnumber(L, q->y);
		return 1;
	}

	if (streq(arg, "w")) {
		lua_pushnumber(L, q->w);
		return 1;
	}

	if (streq(arg, "h")) {
		lua_pushnumber(L, q->h);
		return 1;
	}

	return 0;

}

static int l_quad__newindex(lua_State *L) {

	quad* q = luaL_checkudata(L, 1, "quad");
	const char *arg = luaL_checkstring(L, 2);

	if (streq(arg, "x")) {
		q->x = luaL_checknumber(L, 3);
		return 0;
	}

	if (streq(arg, "y")) {
		q->y = luaL_checknumber(L, 3);
		return 0;
	}

	if (streq(arg, "w")) {
		q->w = luaL_checknumber(L, 3);
		return 0;
	}

	if (streq(arg, "h")) {
		q->h = luaL_checknumber(L, 3);
		return 0;
	}

	return 0;

}

void l_math_init(lua_State *L) {

	luaL_Reg reg[] = {
		{ "vec2", l_vec2f, },
		{ "vec3", l_vec3f, },
		{ "color", l_colorf, },
		{ "quad", l_quadf, },
		{ NULL, NULL },
	};

	luaL_import(L, reg);

	luaL_newmetatable(L, "vec2");
	lua_pushcfunction(L, l_vec2__index);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, l_vec2__newindex);
	lua_setfield(L, -2, "__newindex");
	lua_pushcfunction(L, l_vec2__add);
	lua_setfield(L, -2, "__add");
	lua_pushcfunction(L, l_vec2__sub);
	lua_setfield(L, -2, "__sub");
	lua_pushcfunction(L, l_vec2__mul);
	lua_setfield(L, -2, "__mul");
	lua_pushcfunction(L, l_vec2__div);
	lua_setfield(L, -2, "__div");
	lua_pushcfunction(L, l_vec2__eq);
	lua_setfield(L, -2, "__eq");

	luaL_newmetatable(L, "vec3");
	lua_pushcfunction(L, l_vec3__index);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, l_vec3__newindex);
	lua_setfield(L, -2, "__newindex");
	lua_pushcfunction(L, l_vec3__add);
	lua_setfield(L, -2, "__add");
	lua_pushcfunction(L, l_vec3__sub);
	lua_setfield(L, -2, "__sub");
	lua_pushcfunction(L, l_vec3__mul);
	lua_setfield(L, -2, "__mul");
	lua_pushcfunction(L, l_vec3__div);
	lua_setfield(L, -2, "__div");
	lua_pushcfunction(L, l_vec3__eq);
	lua_setfield(L, -2, "__eq");

	luaL_newmetatable(L, "color");
	lua_pushcfunction(L, l_color__index);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, l_color__newindex);
	lua_setfield(L, -2, "__newindex");
	lua_pushcfunction(L, l_color__eq);
	lua_setfield(L, -2, "__eq");

	luaL_newmetatable(L, "quad");
	lua_pushcfunction(L, l_quad__index);
	lua_setfield(L, -2, "__index");
	lua_pushcfunction(L, l_quad__newindex);
	lua_setfield(L, -2, "__newindex");

}


