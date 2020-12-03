// wengwengweng

#include <lua/lauxlib.h>
#include <dirty/dirty.h>

#include "utils.h"

static int l_vec2(lua_State *L) {

	int num_args = lua_gettop(L);

	if (num_args == 0) {

		vec2 p = vec2f(0.0, 0.0);
		lua_pushudata(L, vec2, &p);
		return 1;

	} else if (num_args == 1) {

		if (lua_isnumber(L, 1)) {

			float v = luaL_checknumber(L, 1);
			vec2 p = vec2f(v, v);
			lua_pushudata(L, vec2, &p);
			return 1;

		} else if (lua_isuserdata(L, 1)) {

			const char *t = lua_udatatype(L, 1);

			if (streq(t, "vec2")) {
				vec2 *p2 = luaL_checkudata(L, 1, "vec2");
				vec2 p = vec2f(p2->x, p2->y);
				lua_pushudata(L, vec2, &p);
				return 1;
			} else if (streq(t, "vec3")) {
				vec3 *p2 = luaL_checkudata(L, 1, "vec3");
				vec2 p = vec2f(p2->x, p2->y);
				lua_pushudata(L, vec2, &p);
				return 1;
			}

		}

	} else if (num_args == 2) {

		float x = luaL_checknumber(L, 1);
		float y = luaL_checknumber(L, 2);
		vec2 p = vec2f(x, y);
		lua_pushudata(L, vec2, &p);
		return 1;

	} else {
		luaL_error(L, "wrong number of arguments to 'vec3()'\n");
	}

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
	} else if (streq(arg, "y")) {
		lua_pushnumber(L, p->y);
		return 1;
	} else if (streq(arg, "len")) {
		lua_pushcfunction(L, l_vec2_len);
		return 1;
	} else if (streq(arg, "unit")) {
		lua_pushcfunction(L, l_vec2_unit);
		return 1;
	} else if (streq(arg, "dist")) {
		lua_pushcfunction(L, l_vec2_dist);
		return 1;
	} else if (streq(arg, "dot")) {
		lua_pushcfunction(L, l_vec2_dot);
		return 1;
	} else {
		luaL_error(L, "unknown member '%s' of 'vec2'\n", arg);
	}

	return 0;

}

static int l_vec2__newindex(lua_State *L) {

	vec2 *p = luaL_checkudata(L, 1, "vec2");
	const char *arg = luaL_checkstring(L, 2);

	if (streq(arg, "x")) {
		p->x = luaL_checknumber(L, 3);
		return 0;
	} if (streq(arg, "y")) {
		p->y = luaL_checknumber(L, 3);
		return 0;
	} else {
		luaL_error(L, "unknown member '%s' of 'vec2'\n", arg);
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

static int l_vec2__unm(lua_State *L) {
	vec2 *p = luaL_checkudata(L, 1, "vec2");
	vec2 np = vec2f(-p->x, -p->y);
	lua_pushudata(L, vec2, &np);
	return 1;
}

static int l_vec2__eq(lua_State *L) {
	vec2 *p1 = luaL_checkudata(L, 1, "vec2");
	vec2 *p2 = luaL_checkudata(L, 2, "vec2");
	lua_pushboolean(L, vec2_eq(*p1, *p2));
	return 1;
}

static int l_vec2__tostring(lua_State *L) {
	vec2 *p = luaL_checkudata(L, 1, "vec2");
	lua_pushstring(L, vec2_fmt(*p));
	return 1;
}

static int l_vec3(lua_State *L) {

	int num_args = lua_gettop(L);

	if (num_args == 0) {

		vec3 p = vec3f(0.0, 0.0, 0.0);
		lua_pushudata(L, vec3, &p);
		return 1;

	} else if (num_args == 1) {

		if (lua_isnumber(L, 1)) {

			float v = luaL_checknumber(L, 1);
			vec3 p = vec3f(v, v, v);
			lua_pushudata(L, vec3, &p);
			return 1;

		} else if (lua_isuserdata(L, 1)) {

			const char *t = lua_udatatype(L, 1);

			if (streq(t, "vec3")) {
				vec3 *p2 = luaL_checkudata(L, 1, "vec3");
				vec3 p = vec3f(p2->x, p2->y, p2->z);
				lua_pushudata(L, vec3, &p);
				return 1;
			} else if (streq(t, "vec2")) {
				vec2 *p2 = luaL_checkudata(L, 1, "vec2");
				vec3 p = vec3f(p2->x, p2->y, 0.0);
				lua_pushudata(L, vec3, &p);
				return 1;
			}

		}

	} else if (num_args == 2) {

		if (lua_isuserdata(L, 1)) {
			vec2 *p2 = luaL_checkudata(L, 1, "vec2");
			float z = luaL_optnumber(L, 2, 0.0);
			vec3 p = vec3f(p2->x, p2->y, z);
			lua_pushudata(L, vec3, &p);
			return 1;
		} else {
			float x = luaL_checknumber(L, 1);
			float y = luaL_checknumber(L, 2);
			vec3 p = vec3f(x, y, 0.0);
			lua_pushudata(L, vec3, &p);
			return 1;
		}

	} else if (num_args == 3) {

		float x = luaL_checknumber(L, 1);
		float y = luaL_checknumber(L, 2);
		float z = luaL_checknumber(L, 3);
		vec3 p = vec3f(x, y, z);
		lua_pushudata(L, vec3, &p);
		return 1;

	} else {
		luaL_error(L, "wrong number of arguments to 'vec3()'\n");
	}

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
	} else if (streq(arg, "y")) {
		lua_pushnumber(L, p->y);
		return 1;
	} else if (streq(arg, "z")) {
		lua_pushnumber(L, p->z);
		return 1;
	} else if (streq(arg, "len")) {
		lua_pushcfunction(L, l_vec3_len);
		return 1;
	} else if (streq(arg, "unit")) {
		lua_pushcfunction(L, l_vec3_unit);
		return 1;
	} else if (streq(arg, "dist")) {
		lua_pushcfunction(L, l_vec3_dist);
		return 1;
	} else if (streq(arg, "dot")) {
		lua_pushcfunction(L, l_vec3_dot);
		return 1;
	} else {
		luaL_error(L, "unknown member '%s' of 'vec3'\n", arg);
	}

	return 0;

}

static int l_vec3__newindex(lua_State *L) {

	vec3 *p = luaL_checkudata(L, 1, "vec3");
	const char *arg = luaL_checkstring(L, 2);

	if (streq(arg, "x")) {
		p->x = luaL_checknumber(L, 3);
		return 0;
	} if (streq(arg, "y")) {
		p->y = luaL_checknumber(L, 3);
		return 0;
	} if (streq(arg, "z")) {
		p->z = luaL_checknumber(L, 3);
		return 0;
	} else {
		luaL_error(L, "unknown member '%s' of 'vec3'\n", arg);
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

static int l_vec3__unm(lua_State *L) {
	vec3 *p = luaL_checkudata(L, 1, "vec3");
	vec3 np = vec3f(-p->x, -p->y, -p->z);
	lua_pushudata(L, vec3, &np);
	return 1;
}

static int l_vec3__eq(lua_State *L) {
	vec3 *p1 = luaL_checkudata(L, 1, "vec3");
	vec3 *p2 = luaL_checkudata(L, 2, "vec3");
	lua_pushboolean(L, vec3_eq(*p1, *p2));
	return 1;
}

static int l_vec3__tostring(lua_State *L) {
	vec3 *p = luaL_checkudata(L, 1, "vec3");
	lua_pushstring(L, vec3_fmt(*p));
	return 1;
}

static int l_color(lua_State *L) {

	int num_args = lua_gettop(L);

	if (num_args == 0) {
		color c = coloru();
		lua_pushudata(L, color, &c);
		return 1;
	} else if (num_args == 1 || num_args == 2) {
		int rgb = luaL_checkinteger(L, 1);
		unsigned char a = luaL_optinteger(L, 2, 255);
		color c = colorx(rgb, a);
		lua_pushudata(L, color, &c);
		return 1;
	} else if (num_args == 3 || num_args == 4) {
		unsigned char r = luaL_checkinteger(L, 1);
		unsigned char g = luaL_checkinteger(L, 2);
		unsigned char b = luaL_checkinteger(L, 3);
		unsigned char a = luaL_optinteger(L, 4, 255);
		color c = colori(r, g, b, a);
		lua_pushudata(L, color, &c);
		return 1;
	} else {
		luaL_error(L, "wrong number of arguments to 'color()'\n");
	}

	return 1;

}

static int l_color__index(lua_State *L) {

	color *c = luaL_checkudata(L, 1, "color");
	const char *arg = luaL_checkstring(L, 2);

	if (streq(arg, "r")) {
		lua_pushnumber(L, c->r);
		return 1;
	} else if (streq(arg, "g")) {
		lua_pushnumber(L, c->g);
		return 1;
	} else if (streq(arg, "b")) {
		lua_pushnumber(L, c->b);
		return 1;
	} else if (streq(arg, "a")) {
		lua_pushnumber(L, c->a);
		return 1;
	} else {
		luaL_error(L, "unknown member '%s' of 'color'\n", arg);
	}

	return 0;

}

static int l_color__newindex(lua_State *L) {

	color *c = luaL_checkudata(L, 1, "color");
	const char *arg = luaL_checkstring(L, 2);

	if (streq(arg, "r")) {
		c->r = luaL_checknumber(L, 3);
		return 0;
	} else if (streq(arg, "g")) {
		c->g = luaL_checknumber(L, 3);
		return 0;
	} else if (streq(arg, "b")) {
		c->b = luaL_checknumber(L, 3);
		return 0;
	} else if (streq(arg, "a")) {
		c->a = luaL_checknumber(L, 3);
		return 0;
	} else {
		luaL_error(L, "unknown member '%s' of 'd_ftext'\n", arg);
	}

	return 0;

}

static int l_color__eq(lua_State *L) {
	color *c1 = luaL_checkudata(L, 1, "color");
	color *c2 = luaL_checkudata(L, 2, "color");
	lua_pushboolean(L, color_eq(*c1, *c2));
	return 1;
}

static int l_color__tostring(lua_State *L) {
	color *c = luaL_checkudata(L, 1, "color");
	lua_pushstring(L, color_fmt(*c));
	return 1;
}

static int l_quad(lua_State *L) {
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
	} else if (streq(arg, "y")) {
		lua_pushnumber(L, q->y);
		return 1;
	} else if (streq(arg, "w")) {
		lua_pushnumber(L, q->w);
		return 1;
	} else if (streq(arg, "h")) {
		lua_pushnumber(L, q->h);
		return 1;
	} else {
		luaL_error(L, "unknown member '%s' of 'quad'\n", arg);
	}

	return 0;

}

static int l_quad__newindex(lua_State *L) {

	quad *q = luaL_checkudata(L, 1, "quad");
	const char *arg = luaL_checkstring(L, 2);

	if (streq(arg, "x")) {
		q->x = luaL_checknumber(L, 3);
		return 0;
	} else if (streq(arg, "y")) {
		q->y = luaL_checknumber(L, 3);
		return 0;
	} else if (streq(arg, "w")) {
		q->w = luaL_checknumber(L, 3);
		return 0;
	} else if (streq(arg, "h")) {
		q->h = luaL_checknumber(L, 3);
		return 0;
	} else {
		luaL_error(L, "unknown member '%s' of 'quad'\n", arg);
	}

	return 0;

}

static int l_quad__tostring(lua_State *L) {
	quad *q = luaL_checkudata(L, 1, "quad");
	lua_pushstring(L, quad_fmt(*q));
	return 1;
}

static int l_mat4(lua_State *L) {

	mat4 m = mat4u();

	if (!lua_isnoneornil(L, 1)) {

		luaL_checktable(L, 1);
		int num = lua_rawlen(L, 1);

		if (num != 16) {
			luaL_error(L, "wrong number of 'mat4' members\n");
		}

		for (int i = 0; i < num; i++) {
			lua_rawgeti(L, 1, i + 1);
			float v = luaL_checknumber(L, -1);
			m.m[i] = v;
			lua_pop(L, 1);
		}

	}

	lua_pushudata(L, mat4, &m);

	return 1;

}

static int l_mat4_invert(lua_State *L) {
	mat4 *m = luaL_checkudata(L, 1, "mat4");
	mat4 res = mat4_invert(*m);
	lua_pushudata(L, mat4, &res);
	return 1;
}

static int l_mat4__index(lua_State *L) {

	mat4 *m = luaL_checkudata(L, 1, "mat4");

	if (lua_isinteger(L, 2)) {
		int i = luaL_checkinteger(L, 2);
		if (i < 1 || i > 16) {
			luaL_error(L, "'mat4' does not contain index '%d'\n", i);
		}
		lua_pushnumber(L, m->m[i - 1]);
		return 1;
	}

	if (lua_isstring(L, 2)) {
		const char *arg = luaL_checkstring(L, 2);
		if (streq(arg, "invert")) {
			lua_pushcfunction(L, l_mat4_invert);
			return 1;
		}
	}

	return 0;

}

static int l_mat4__newindex(lua_State *L) {

	mat4 *m = luaL_checkudata(L, 1, "mat4");
	int i = luaL_checkinteger(L, 2);
	float v = luaL_checknumber(L, 3);

	if (i < 1 || i > 16) {
		luaL_error(L, "'mat4' does not contain index '%d'\n", i);
	}

	m->m[i - 1] = v;

	return 0;

}

static int l_mat4__mul(lua_State *L) {
	mat4 *m = luaL_checkudata(L, 1, "mat4");
	const char *t = lua_udatatype(L, 2);
	if (streq(t, "mat4")) {
		mat4 *m2 = luaL_checkudata(L, 2, "mat4");
		mat4 res = mat4_mult(*m, *m2);
		lua_pushudata(L, mat4, &res);
		return 1;
	} else if (streq(t, "vec4")) {
		vec4 *p = luaL_checkudata(L, 2, "vec4");
		vec4 res = mat4_mult_vec4(*m, *p);
		lua_pushudata(L, vec4, &res);
		return 1;
	} else if (streq(t, "vec3")) {
		vec3 *p = luaL_checkudata(L, 2, "vec3");
		vec3 res = mat4_mult_vec3(*m, *p);
		lua_pushudata(L, vec3, &res);
		return 1;
	} else if (streq(t, "vec2")) {
		vec2 *p = luaL_checkudata(L, 2, "vec2");
		vec2 res = mat4_mult_vec2(*m, *p);
		lua_pushudata(L, vec2, &res);
		return 1;
	} else {
		luaL_error(L, "bad matrix multiplication argument\n");
	}
	return 0;
}

static int l_mat4__tostring(lua_State *L) {
	mat4 *m = luaL_checkudata(L, 1, "mat4");
	lua_pushstring(L, mat4_fmt(*m));
	return 1;
}

void l_math_init(lua_State *L) {

	luaL_regfuncs(L, (luaL_Reg[]) {
		{ "vec2", l_vec2, },
		{ "vec3", l_vec3, },
		{ "color", l_color, },
		{ "quad", l_quad, },
		{ "mat4", l_mat4, },
		{ NULL, NULL, },
	});

	luaL_regtype(L, "vec2", (luaL_Reg[]) {
		{ "__index", l_vec2__index, },
		{ "__newindex", l_vec2__newindex, },
		{ "__add", l_vec2__add, },
		{ "__sub", l_vec2__sub, },
		{ "__mul", l_vec2__mul, },
		{ "__div", l_vec2__div, },
		{ "__unm", l_vec2__unm, },
		{ "__eq", l_vec2__eq, },
		{ "__tostring", l_vec2__tostring, },
		{ NULL, NULL, },
	});

	luaL_regtype(L, "vec3", (luaL_Reg[]) {
		{ "__index", l_vec3__index, },
		{ "__newindex", l_vec3__newindex, },
		{ "__add", l_vec3__add, },
		{ "__sub", l_vec3__sub, },
		{ "__mul", l_vec3__mul, },
		{ "__div", l_vec3__div, },
		{ "__unm", l_vec3__unm, },
		{ "__eq", l_vec3__eq, },
		{ "__tostring", l_vec3__tostring, },
		{ NULL, NULL, },
	});

	luaL_regtype(L, "color", (luaL_Reg[]) {
		{ "__index", l_color__index, },
		{ "__newindex", l_color__newindex, },
		{ "__eq", l_color__eq, },
		{ "__tostring", l_color__tostring, },
		{ NULL, NULL, },
	});

	luaL_regtype(L, "quad", (luaL_Reg[]) {
		{ "__index", l_quad__index, },
		{ "__newindex", l_quad__newindex, },
		{ "__tostring", l_quad__tostring, },
		{ NULL, NULL, },
	});

	luaL_regtype(L, "mat4", (luaL_Reg[]) {
		{ "__index", l_mat4__index, },
		{ "__newindex", l_mat4__newindex, },
		{ "__mul", l_mat4__mul, },
		{ "__tostring", l_mat4__tostring, },
		{ NULL, NULL, },
	});

}

