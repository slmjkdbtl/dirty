// wengwengweng

#include <lua/lauxlib.h>
#include <dirty/dirty.h>

#include "utils.h"

d_tex_conf l_parse_tex_conf(lua_State *L, int index) {

	lua_getfield(L, index, "filter");
	d_tex_filter filter = luaL_optinteger(L, -1, D_NEAREST);
	lua_pop(L, 1);

	lua_getfield(L, index, "wrap");
	d_tex_wrap wrap = luaL_optinteger(L, -1, D_REPEAT);
	lua_pop(L, 1);

	return (d_tex_conf) {
		.filter = filter,
		.wrap = wrap,
	};

}

d_vertex l_parse_vertex(lua_State *L, int index) {

	lua_getfield(L, index, "pos");
	vec3 pos = *(vec3*)luaL_checkudata(L, -1, "vec3");
	lua_pop(L, 1);

	lua_getfield(L, index, "normal");
	vec3 normal = *(vec3*)luaL_checkudata(L, -1, "vec3");
	lua_pop(L, 1);

	lua_getfield(L, index, "uv");
	vec2 uv = *(vec2*)luaL_checkudata(L, -1, "vec2");
	lua_pop(L, 1);

	lua_getfield(L, index, "color");
	color c = *(color*)luaL_checkudata(L, -1, "color");
	lua_pop(L, 1);

	return (d_vertex) {
		.pos = pos,
		.normal = normal,
		.uv = uv,
		.color = c,
	};
}

d_cam l_parse_cam(lua_State *L, int index) {

	lua_getfield(L, index, "proj");
	mat4 proj = *(mat4*)luaL_checkudata(L, -1, "mat4");
	lua_pop(L, 1);

	lua_getfield(L, index, "view");
	mat4 view = *(mat4*)luaL_checkudata(L, -1, "mat4");
	lua_pop(L, 1);

	return (d_cam) {
		.proj = proj,
		.view = view,
	};
}

d_vertex *l_parse_vertex_list(lua_State *L, int index) {
	int num_verts = lua_rawlen(L, index);
	d_vertex *verts = malloc(sizeof(d_vertex) * num_verts);
	for (int i = 0; i < num_verts; i++) {
		lua_rawgeti(L, index, i + 1);
		verts[i] = l_parse_vertex(L, -1);
		lua_pop(L, 1);
	}
	return verts;
}

d_index *l_parse_index_list(lua_State *L, int index) {
	int num_indices = lua_rawlen(L, index);
	d_index *indices = malloc(sizeof(d_index) * num_indices);
	for (int i = 0; i < num_indices; i++) {
		lua_rawgeti(L, index, i + 1);
		indices[i] = luaL_checkinteger(L, 3);
		lua_pop(L, 1);
	}
	return indices;
}

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

static int l_transform(lua_State *L) {
	mat4 m = d_transform();
	lua_pushudata(L, mat4, &m);
	return 1;
}

static int l_coord(lua_State *L) {
	d_origin orig = luaL_checkinteger(L, 1);
	vec2 pos = d_coord(orig);
	lua_pushudata(L, vec2, &pos);
	return 1;
}

static int l_mouse_pos_t(lua_State *L) {
	vec2 mpos = d_mouse_pos_t();
	lua_pushudata(L, vec2, &mpos);
	return 1;
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

static int l_move_x(lua_State *L) {
	float x = luaL_checknumber(L, 1);
	d_move_x(x);
	return 0;
}

static int l_move_y(lua_State *L) {
	float y = luaL_checknumber(L, 1);
	d_move_y(y);
	return 0;
}

static int l_move_z(lua_State *L) {
	float z = luaL_checknumber(L, 1);
	d_move_z(z);
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

static int l_scale_x(lua_State *L) {
	float x = luaL_checknumber(L, 1);
	d_scale_x(x);
	return 0;
}

static int l_scale_y(lua_State *L) {
	float y = luaL_checknumber(L, 1);
	d_scale_y(y);
	return 0;
}

static int l_scale_z(lua_State *L) {
	float z = luaL_checknumber(L, 1);
	d_scale_z(z);
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
	int slot = luaL_checkinteger(L, 2);
	d_tex *t = luaL_checkudata(L, 3, "d_tex");
	d_send_tex(name, slot, t);
	return 0;
}

static int l_make_shader(lua_State *L) {
	const char *vs = luaL_optstring(L, 1, NULL);
	const char *fs = luaL_optstring(L, 2, NULL);
	d_shader p = d_make_shader(vs, fs);
	lua_pushudata(L, d_shader, &p);
	return 1;
}

static int l_load_shader(lua_State *L) {
	const char *vspath = luaL_optstring(L, 1, NULL);
	const char *fspath = luaL_optstring(L, 2, NULL);
	d_shader p = d_load_shader(vspath, fspath);
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
	int w = luaL_checkinteger(L, 1);
	int h = luaL_checkinteger(L, 2);
	d_canvas c = d_make_canvas(w, h);
	lua_pushudata(L, d_canvas, &c);
	return 1;
}

static int l_make_canvas_ex(lua_State *L) {
	int w = luaL_checkinteger(L, 1);
	int h = luaL_checkinteger(L, 2);
	luaL_checktable(L, 3);
	d_tex_conf conf = l_parse_tex_conf(L, 3);
	d_canvas c = d_make_canvas_ex(w, h, conf);
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
		lua_pushinteger(L, c->width);
		return 1;
	}

	if (streq(arg, "height")) {
		lua_pushinteger(L, c->height);
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

static int l_load_tex_ex(lua_State *L) {
	const char *path = luaL_checkstring(L, 1);
	luaL_checktable(L, 2);
	d_tex_conf conf = l_parse_tex_conf(L, 2);
	d_tex t = d_load_tex_ex(path, conf);
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
		lua_pushinteger(L, t->width);
		return 1;
	}

	if (streq(arg, "height")) {
		lua_pushinteger(L, t->height);
		return 1;
	}

	if (streq(arg, "free")) {
		lua_pushcfunction(L, l_free_tex);
		return 1;
	}

	return 0;

}

static int l_make_mesh(lua_State *L) {

	luaL_checktable(L, 1);
	luaL_checktable(L, 2);
	int num_verts = lua_rawlen(L, 1);
	int num_indices = lua_rawlen(L, 2);
	d_vertex *verts = l_parse_vertex_list(L, 1);
	d_index *indices = l_parse_index_list(L, 2);

	d_mesh m = d_make_mesh(verts, num_verts, indices, num_indices);
	free(verts);
	free(indices);
	lua_pushudata(L, d_mesh, &m);

	return 1;
}

static int l_free_mesh(lua_State *L) {
	d_mesh *m = luaL_checkudata(L, 1, "d_mesh");
	d_free_mesh(m);
	return 0;
}

static int l_mesh__index(lua_State *L) {

	const char *arg = luaL_checkstring(L, 2);

	if (streq(arg, "free")) {
		lua_pushcfunction(L, l_free_mesh);
		return 1;
	}

	return 0;

}

static int l_make_font(lua_State *L) {
	d_tex *tex = luaL_checkudata(L, 1, "d_tex");
	int gw = luaL_checkinteger(L, 2);
	int gh = luaL_checkinteger(L, 3);
	const char *chars = luaL_optstring(L, 4, D_ASCII_CHARS);
	d_font f = d_make_font(*tex, gw, gh, chars);
	lua_pushudata(L, d_font, &f);
	return 1;
}

static int l_free_font(lua_State *L) {
	d_font *f = luaL_checkudata(L, 1, "d_font");
	d_free_font(f);
	return 0;
}

static int l_font__index(lua_State *L) {

	const char *arg = luaL_checkstring(L, 2);

	if (streq(arg, "free")) {
		lua_pushcfunction(L, l_free_font);
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

static int l_use_font(lua_State *L) {
	if (lua_isnoneornil(L, 1)) {
		d_use_font(NULL);
	} else {
		d_font *f = luaL_checkudata(L, 1, "d_font");
		d_use_font(f);
	}
	return 0;
}

static int l_use_cam(lua_State *L) {
	if (lua_isnoneornil(L, 1)) {
		d_use_cam(NULL);
	} else {
		d_cam *c = luaL_checkudata(L, 1, "d_cam");
		d_use_cam(c);
	}
	return 0;
}

static int l_fmt_text(lua_State *L) {
	const char *str = luaL_checkstring(L, 1);
	float size = luaL_checknumber(L, 2);
	float wrap = luaL_optnumber(L, 3, 0.0);
	d_origin orig = luaL_optinteger(L, 4, D_CENTER);
	color c = lua_isnoneornil(L, 5) ? coloru() : *(color*)luaL_checkudata(L, 5, "color");
	d_ftext t = d_fmt_text(str, size, wrap, orig, c);
	lua_pushudata(L, d_ftext, &t);
	return 1;
}

static int l_ftext_cpos(lua_State *L) {
	d_ftext *t = luaL_checkudata(L, 1, "d_ftext");
	int n = luaL_checkinteger(L, 2);
	vec2 pos = d_ftext_cpos(t, n);
	lua_pushudata(L, vec2, &pos);
	return 1;
}

static int l_ftext__index(lua_State *L) {

	d_ftext *t = luaL_checkudata(L, 1, "d_ftext");
	const char *arg = luaL_checkstring(L, 2);

	if (streq(arg, "width")) {
		lua_pushnumber(L, t->width);
		return 1;
	}

	if (streq(arg, "height")) {
		lua_pushnumber(L, t->height);
		return 1;
	}

	if (streq(arg, "cpos")) {
		lua_pushcfunction(L, l_ftext_cpos);
		return 1;
	}

	return 0;

}

static int l_draw_raw(lua_State *L) {

	luaL_checktable(L, 1);
	luaL_checktable(L, 2);
	int num_verts = lua_rawlen(L, 1);
	int num_indices = lua_rawlen(L, 2);
	d_vertex *verts = l_parse_vertex_list(L, 1);
	d_index *indices = l_parse_index_list(L, 2);

	d_draw_raw(verts, num_verts, indices, num_indices, NULL);
	free(verts);
	free(indices);

	return 0;

}

static int l_draw_mesh(lua_State *L) {
	d_mesh *m = luaL_checkudata(L, 1, "d_mesh");
	d_draw_mesh(m);
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
	d_origin orig = luaL_optinteger(L, 4, D_CENTER);
	color c = lua_isnoneornil(L, 5) ? coloru() : *(color*)luaL_checkudata(L, 5, "color");

	d_draw_text(str, size, wrap, orig, c);

	return 0;

}

static int l_draw_ftext(lua_State *L) {
	d_ftext *t = luaL_checkudata(L, 1, "d_ftext");
	d_draw_ftext(t);
	return 0;
}

static int l_draw_canvas(lua_State *L) {

	d_canvas *c = luaL_checkudata(L, 1, "d_canvas");
	color co = lua_isnoneornil(L, 2) ? coloru() : *(color*)luaL_checkudata(L, 2, "color");

	d_draw_canvas(c, co);

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
	float w = luaL_checknumber(L, 3);
	color c = lua_isnoneornil(L, 4) ? coloru() : *(color*)luaL_checkudata(L, 4, "color");

	d_draw_line(*p1, *p2, w, c);

	return 0;

}

static int l_draw_lrect(lua_State *L) {

	vec2 *p1 = luaL_checkudata(L, 1, "vec2");
	vec2 *p2 = luaL_checkudata(L, 2, "vec2");
	float w = luaL_checknumber(L, 3);
	color c = lua_isnoneornil(L, 4) ? coloru() : *(color*)luaL_checkudata(L, 4, "color");

	d_draw_lrect(*p1, *p2, w, c);

	return 0;

}

static int l_draw_circle(lua_State *L) {

	vec2 *center = luaL_checkudata(L, 1, "vec2");
	float r = luaL_checknumber(L, 2);
	color c = lua_isnoneornil(L, 3) ? coloru() : *(color*)luaL_checkudata(L, 3, "color");

	d_draw_circle(*center, r, c);

	return 0;

}

static int l_gl_version(lua_State *L) {
	lua_pushstring(L, d_gl_version());
	return 1;
}

static int l_glsl_version(lua_State *L) {
	lua_pushstring(L, d_glsl_version());
	return 1;
}

static int l_gl_renderer(lua_State *L) {
	lua_pushstring(L, d_gl_renderer());
	return 1;
}

static int l_gl_vendor(lua_State *L) {
	lua_pushstring(L, d_gl_vendor());
	return 1;
}

void l_gfx_init(lua_State *L) {

	luaL_regfuncs(L, (luaL_Reg[]) {
		{ "d_clear", l_clear, },
		{ "d_clear_color", l_clear_color, },
		{ "d_clear_depth", l_clear_depth, },
		{ "d_clear_stencil", l_clear_stencil, },
		{ "d_depth_write", l_depth_write, },
		{ "d_depth_test", l_depth_test, },
		{ "d_stencil_write", l_stencil_write, },
		{ "d_stencil_test", l_stencil_test, },
		{ "d_transform", l_transform, },
		{ "d_coord", l_coord, },
		{ "d_mouse_pos_t", l_mouse_pos_t, },
		{ "d_push", l_push, },
		{ "d_pop", l_pop, },
		{ "d_move", l_move, },
		{ "d_move_x", l_move_x, },
		{ "d_move_y", l_move_y, },
		{ "d_move_z", l_move_z, },
		{ "d_move_xy", l_move_xy, },
		{ "d_scale", l_scale, },
		{ "d_scale_xy", l_scale_xy, },
		{ "d_scale_x", l_scale_x, },
		{ "d_scale_y", l_scale_y, },
		{ "d_scale_z", l_scale_z, },
		{ "d_rot_x", l_rot_x, },
		{ "d_rot_y", l_rot_y, },
		{ "d_rot_z", l_rot_z, },
		{ "d_send_f", l_send_f, },
		{ "d_send_vec2", l_send_vec2, },
		{ "d_send_vec3", l_send_vec3, },
		{ "d_send_color", l_send_color, },
		{ "d_send_tex", l_send_tex, },
		{ "d_make_mesh", l_make_mesh, },
		{ "d_make_font", l_make_font, },
		{ "d_make_shader", l_make_shader, },
		{ "d_load_shader", l_load_shader, },
		{ "d_make_canvas", l_make_canvas, },
		{ "d_make_canvas_ex", l_make_canvas_ex, },
		{ "d_load_tex", l_load_tex, },
		{ "d_load_tex_ex", l_load_tex_ex, },
		{ "d_use_shader", l_use_shader, },
		{ "d_use_canvas", l_use_canvas, },
		{ "d_use_font", l_use_font, },
		{ "d_use_cam", l_use_cam, },
		{ "d_fmt_text", l_fmt_text, },
		{ "d_draw_raw", l_draw_raw, },
		{ "d_draw_mesh", l_draw_mesh, },
		{ "d_draw_tex", l_draw_tex, },
		{ "d_draw_text", l_draw_text, },
		{ "d_draw_ftext", l_draw_ftext, },
		{ "d_draw_canvas", l_draw_canvas, },
		{ "d_draw_rect", l_draw_rect, },
		{ "d_draw_line", l_draw_line, },
		{ "d_draw_lrect", l_draw_lrect, },
		{ "d_draw_circle", l_draw_circle, },
		{ "d_gl_version", l_gl_version, },
		{ "d_glsl_version", l_glsl_version, },
		{ "d_gl_renderer", l_gl_renderer, },
		{ "d_gl_vendor", l_gl_vendor, },
		{ NULL, NULL, },
	});

	luaL_regenum(L, "d_origin", (luaL_Enum[]) {
		{ "topleft", D_TOP_LEFT, },
		{ "top", D_TOP, },
		{ "topright", D_TOP_RIGHT, },
		{ "left", D_LEFT, },
		{ "center", D_CENTER, },
		{ "right", D_RIGHT, },
		{ "botleft", D_BOT_LEFT, },
		{ "bot", D_BOT, },
		{ "botright", D_BOT_RIGHT, },
		{ NULL, 0, },
	});

	luaL_regenum(L, "d_tex_filter", (luaL_Enum[]) {
		{ "nearest", D_NEAREST, },
		{ "linear", D_LINEAR, },
		{ NULL, 0, },
	});

	luaL_regenum(L, "d_tex_wrap", (luaL_Enum[]) {
		{ "repeat", D_REPEAT, },
		{ "mirroredrepeat", D_MIRRORED_REPEAT, },
		{ "clamptoedge", D_CLAMP_TO_EDGE, },
		{ "clamptoborder", D_CLAMP_TO_BORDER, },
		{ NULL, 0, },
	});

	luaL_regenum(L, "d_blend", (luaL_Enum[]) {
		{ "alpha", D_BLEND_ALPHA, },
		{ "add", D_BLEND_ADD, },
		{ "replace", D_BLEND_REPLACE, },
		{ NULL, 0, },
	});

	luaL_regtype(L, "d_mesh", (luaL_Reg[]) {
		{ "__index", l_mesh__index, },
		{ NULL, NULL, },
	});

	luaL_regtype(L, "d_shader", (luaL_Reg[]) {
		{ "__index", l_shader__index, },
		{ NULL, NULL, },
	});

	luaL_regtype(L, "d_canvas", (luaL_Reg[]) {
		{ "__index", l_canvas__index, },
		{ NULL, NULL, },
	});

	luaL_regtype(L, "d_tex", (luaL_Reg[]) {
		{ "__index", l_tex__index, },
		{ NULL, NULL, },
	});

	luaL_regtype(L, "d_font", (luaL_Reg[]) {
		{ "__index", l_font__index, },
		{ NULL, NULL, },
	});

	luaL_regtype(L, "d_ftext", (luaL_Reg[]) {
		{ "__index", l_ftext__index, },
		{ NULL, NULL, },
	});

}

