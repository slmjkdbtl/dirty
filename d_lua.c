#include <string.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

#define D_IMPL
#include <d_plat.h>
#include <d_fs.h>
#include <d_math.h>
#include <d_tween.h>
#include <d_app.h>
#include <d_gfx.h>
#define STB_VORBIS_IMPLEMENTATION
#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>
#include <d_audio.h>

#define lua_pushudata(L, T, TS, V) \
	T* lv = lua_newuserdata(L, sizeof(T)); \
	luaL_setmetatable(L, TS); \
	memcpy(lv, V, sizeof(T)); \

#define luaL_setfieldudata(L, IDX, K, T, TS, V) \
	lua_pushudata(L, T, TS, &V); \
	lua_setfield(L, IDX, K); \

typedef struct {
	const char *str;
	int val;
} luaL_Enum;

bool luaL_args0(lua_State* L) {
	return lua_gettop(L) == 0;
}

bool luaL_args1(lua_State* L, int type) {
	if (lua_gettop(L) == 1) {
		if (lua_type(L, 1) == type) {
			return true;
		}
	}
	return false;
}

bool luaL_args2(lua_State* L, int t1, int t2) {
	if (lua_gettop(L) == 2) {
		if (lua_type(L, 1) == t1 && lua_type(L, 2) == t2) {
			return true;
		}
	}
	return false;
}

bool luaL_args3(lua_State* L, int t1, int t2, int t3) {
	if (lua_gettop(L) == 3) {
		if (
			lua_type(L, 1) == t1
			&& lua_type(L, 2) == t2
			&& lua_type(L, 3) == t3
		) {
			return true;
		}
	}
	return false;
}

bool luaL_args4(lua_State* L, int t1, int t2, int t3, int t4) {
	if (lua_gettop(L) == 4) {
		if (
			lua_type(L, 1) == t1
			&& lua_type(L, 2) == t2
			&& lua_type(L, 3) == t3
			&& lua_type(L, 4) == t4
		) {
			return true;
		}
	}
	return false;
}

static void luaL_stackdump(lua_State *L) {
	int top = lua_gettop(L);
	printf("Lua Stack (size: %d):\n", top);
	for (int i = 1; i <= top; i++) {
		int t = lua_type(L, i);
		switch (t) {
			case LUA_TNUMBER:
				printf("%d: number = %f\n", i, lua_tonumber(L, i));
				break;
			case LUA_TSTRING:
				printf("%d: string = '%s'\n", i, lua_tostring(L, i));
				break;
			case LUA_TBOOLEAN:
				printf("%d: boolean = %s\n", i, lua_toboolean(L, i) ? "true" : "false");
				break;
			case LUA_TNIL:
				printf("%d: nil\n", i);
				break;
			case LUA_TTABLE:
				printf("%d: table\n", i);
				break;
			case LUA_TFUNCTION:
				printf("%d: function\n", i);
				break;
			case LUA_TUSERDATA:
				printf("%d: userdata\n", i);
				break;
			case LUA_TLIGHTUSERDATA:
				printf("%d: lightuserdata\n", i);
				break;
			case LUA_TTHREAD:
				printf("%d: thread\n", i);
				break;
			default:
				printf("%d: unknown type\n", i);
				break;
		}
	}
	printf("\n");
}

void luaL_checktable(lua_State* L, int pos) {
	luaL_checktype(L, pos, LUA_TTABLE);
}

void luaL_checkfunction(lua_State* L, int pos) {
	luaL_checktype(L, pos, LUA_TFUNCTION);
}

bool luaL_checkboolean(lua_State* L, int pos) {
	luaL_checktype(L, pos, LUA_TBOOLEAN);
	return lua_toboolean(L, pos);
}

bool luaL_optboolean(lua_State* L, int pos, bool def) {
	if (lua_isnoneornil(L, pos)) {
		return def;
	}
	return luaL_checkboolean(L, pos);
}

void* luaL_optudata(lua_State* L, int pos, const char *type, void *def) {
	return lua_isnoneornil(L, pos) ? def : luaL_checkudata(L, pos, type);
}

const char* luaL_udatatype(lua_State* L, int pos) {
	if (!lua_getmetatable(L, pos)) {
		return NULL;
	}
	lua_getfield(L, -1, "__name");
	const char* tname = lua_tostring(L, -1);
	lua_pop(L, 2);
	return tname;
}

static int luaL_tablelen(lua_State* L, int idx) {
	lua_len(L, idx);
	int len = lua_tonumber(L, -1);
	lua_pop(L, 1);
	return len;
}

static int luaL_absidx(lua_State* L, int idx) {
	return idx < 0 ? lua_gettop(L) + idx + 1 : idx;
}

static void luaL_rmi(lua_State* L, int idx, int pos) {

	idx = luaL_absidx(L, idx);

	int len = lua_rawlen(L, idx);

	if (pos < 1 || pos > len) {
		return;
	}

	for (int i = pos; i < len; i++) {
		lua_geti(L, idx, i + 1);
		lua_seti(L, idx, i);
	}

	lua_pushnil(L);
	lua_seti(L, idx, len);

}

void luaL_regfuncs(lua_State* L, luaL_Reg *reg) {
	for (int i = 0; reg[i].name != NULL; i++) {
		lua_register(L, reg[i].name, reg[i].func);
	}
}

int luaL_getenum(lua_State* L, luaL_Enum *map, const char *str) {
	for (int i = 0; map[i].str != NULL; i++) {
		if (strcmp(str, map[i].str) == 0) {
			return map[i].val;
		}
	}
	return 0;
}

void luaL_regenum(lua_State* L, const char* name, luaL_Enum *map) {
	lua_newtable(L);
	for (int i = 0; map[i].str != NULL; i++) {
		lua_pushinteger(L, map[i].val);
		lua_setfield(L, -2, map[i].str);
	}
	lua_setglobal(L, name);
}

void luaL_regtype(lua_State* L, const char* name, luaL_Reg *meta) {
	luaL_newmetatable(L, name);
	for (int i = 0; meta[i].name != NULL; i++) {
		lua_pushstring(L, meta[i].name);
		lua_pushcfunction(L, meta[i].func);
		lua_settable(L, -3);
	}
	lua_pop(L, 1);
}

static int luaL_getfieldtype(lua_State* L, int idx, char* key) {
	lua_getfield(L, idx, key);
	int ty = lua_type(L, -1);
	lua_pop(L, 1);
	return ty;
}

static bool luaL_getfieldboolean(lua_State* L, int idx, char* key) {
	lua_getfield(L, idx, key);
	bool b = luaL_checkboolean(L, -1);
	lua_pop(L, 1);
	return b;
}

static float luaL_getfieldnumber(lua_State* L, int idx, char* key) {
	lua_getfield(L, idx, key);
	float n = luaL_checknumber(L, -1);
	lua_pop(L, 1);
	return n;
}

static const char* luaL_getfieldstring(lua_State* L, int idx, char* key) {
	lua_getfield(L, idx, key);
	const char* str = luaL_checkstring(L, -1);
	lua_pop(L, 1);
	return str;
}

static const char* luaL_getfieldstringopt(lua_State* L, int idx, char* key) {
	lua_getfield(L, idx, key);
	const char* str = NULL;
	if (lua_isstring(L, -1)) {
		str = luaL_checkstring(L, -1);
	}
	lua_pop(L, 1);
	return str;
}

static void* luaL_getfieldudata(lua_State* L, int idx, char* key, char* ty) {
	lua_getfield(L, idx, key);
	void* v = luaL_checkudata(L, -1, ty);
	lua_pop(L, 1);
	return v;
}

static void luaL_setfieldboolean(lua_State* L, int idx, char* key, bool b) {
	lua_pushboolean(L, b);
	lua_setfield(L, idx, key);
}

static void luaL_setfieldnumber(lua_State* L, int idx, char* key, float v) {
	lua_pushnumber(L, v);
	lua_setfield(L, idx, key);
}

static luaL_Enum l_app_mouse_map[] = {
	{ "left", D_MOUSE_LEFT, },
	{ "right", D_MOUSE_RIGHT, },
	{ "middle", D_MOUSE_MIDDLE, },
	{ NULL, 0, },
};

static luaL_Enum l_app_key_map[] = {
	{ "q", D_KEY_Q, },
	{ "w", D_KEY_W, },
	{ "e", D_KEY_E, },
	{ "r", D_KEY_R, },
	{ "t", D_KEY_T, },
	{ "y", D_KEY_Y, },
	{ "u", D_KEY_U, },
	{ "i", D_KEY_I, },
	{ "o", D_KEY_O, },
	{ "p", D_KEY_P, },
	{ "a", D_KEY_A, },
	{ "s", D_KEY_S, },
	{ "d", D_KEY_D, },
	{ "f", D_KEY_F, },
	{ "g", D_KEY_G, },
	{ "h", D_KEY_H, },
	{ "j", D_KEY_J, },
	{ "k", D_KEY_K, },
	{ "l", D_KEY_L, },
	{ "z", D_KEY_Z, },
	{ "x", D_KEY_X, },
	{ "c", D_KEY_C, },
	{ "v", D_KEY_V, },
	{ "b", D_KEY_B, },
	{ "n", D_KEY_N, },
	{ "m", D_KEY_M, },
	{ "1", D_KEY_1, },
	{ "2", D_KEY_2, },
	{ "3", D_KEY_3, },
	{ "4", D_KEY_4, },
	{ "5", D_KEY_5, },
	{ "6", D_KEY_6, },
	{ "7", D_KEY_7, },
	{ "8", D_KEY_8, },
	{ "9", D_KEY_9, },
	{ "0", D_KEY_0, },
	{ "f1", D_KEY_F1, },
	{ "f2", D_KEY_F2, },
	{ "f3", D_KEY_F3, },
	{ "f4", D_KEY_F4, },
	{ "f5", D_KEY_F5, },
	{ "f6", D_KEY_F6, },
	{ "f7", D_KEY_F7, },
	{ "f8", D_KEY_F8, },
	{ "f9", D_KEY_F9, },
	{ "f10", D_KEY_F10, },
	{ "f11", D_KEY_F11, },
	{ "f12", D_KEY_F12, },
	{ "-", D_KEY_MINUS, },
	{ "minus", D_KEY_MINUS, },
	{ "=", D_KEY_EQUAL, },
	{ "equal", D_KEY_EQUAL, },
	{ ",", D_KEY_COMMA, },
	{ "comma", D_KEY_COMMA, },
	{ ".", D_KEY_PERIOD, },
	{ "period", D_KEY_PERIOD, },
	{ "`", D_KEY_BACKTICK, },
	{ "backtick", D_KEY_BACKTICK, },
	{ "/", D_KEY_SLASH, },
	{ "slash", D_KEY_SLASH, },
	{ "\\", D_KEY_BACKSLASH, },
	{ "backslash", D_KEY_BACKSLASH, },
	{ ";", D_KEY_SEMICOLON, },
	{ "semicolon", D_KEY_SEMICOLON, },
	{ "'", D_KEY_QUOTE, },
	{ "quote", D_KEY_QUOTE, },
	{ "up", D_KEY_UP, },
	{ "down", D_KEY_DOWN, },
	{ "left", D_KEY_LEFT, },
	{ "right", D_KEY_RIGHT, },
	{ "esc", D_KEY_ESC, },
	{ "escape", D_KEY_ESC, },
	{ "tab", D_KEY_TAB, },
	{ " ", D_KEY_SPACE, },
	{ "space", D_KEY_SPACE, },
	{ "bksp", D_KEY_BACKSPACE, },
	{ "backspace", D_KEY_BACKSPACE, },
	{ "enter", D_KEY_ENTER, },
	{ "[", D_KEY_LBRACKET, },
	{ "lbracket", D_KEY_LBRACKET, },
	{ "]", D_KEY_RBRACKET, },
	{ "rbracket", D_KEY_RBRACKET, },
	{ "lshft", D_KEY_LSHIFT, },
	{ "rshft", D_KEY_RSHIFT, },
	{ "lalt", D_KEY_LALT, },
	{ "ralt", D_KEY_RALT, },
	{ "lmeta", D_KEY_LMETA, },
	{ "rmeta", D_KEY_RMETA, },
	{ "lctrl", D_KEY_LCTRL, },
	{ "rctrl", D_KEY_RCTRL, },
	{ NULL, 0, },
};

static luaL_Enum l_app_keymod_map[] = {
	{ "alt", D_KEYMOD_ALT, },
	{ "meta", D_KEYMOD_META, },
	{ "ctrl", D_KEYMOD_CTRL, },
	{ "shift", D_KEYMOD_SHIFT, },
	{ NULL, 0, },
};

typedef struct {
	lua_State* lua;
	int app_init_ref;
	int app_frame_ref;
	int app_quit_ref;
	int app_err_ref;
	int gfx_shader_ref;
} l_ctx;

static l_ctx g;

static void l_init_inner(void) {
	lua_rawgeti(g.lua, LUA_REGISTRYINDEX, g.app_init_ref);
	if (lua_isfunction(g.lua, -1)) {
		lua_call(g.lua, 0, 0);
	}
}

static void l_frame_inner(void) {
	lua_rawgeti(g.lua, LUA_REGISTRYINDEX, g.app_frame_ref);
	if (lua_isfunction(g.lua, -1)) {
		lua_call(g.lua, 0, 0);
	}
}

static void l_quit_inner(void) {
	lua_rawgeti(g.lua, LUA_REGISTRYINDEX, g.app_quit_ref);
	if (lua_isfunction(g.lua, -1)) {
		lua_call(g.lua, 0, 0);
	}
}

static int l_app_run(lua_State* L) {

	luaL_checktable(L, 1);

	lua_getfield(L, 1, "init");
	g.app_init_ref = luaL_ref(L, LUA_REGISTRYINDEX);
	lua_getfield(L, 1, "frame");
	g.app_frame_ref = luaL_ref(L, LUA_REGISTRYINDEX);
	lua_getfield(L, 1, "quit");
	g.app_quit_ref = luaL_ref(L, LUA_REGISTRYINDEX);

	d_app_desc desc = {0};
	desc.init = l_init_inner;
	desc.frame = l_frame_inner;
	desc.quit = l_quit_inner;

	// TODO: luaL_check* does not give meaningful error for table fields

	if (lua_getfield(L, -1, "title")) {
		desc.title = luaL_checkstring(L, -1);
	}
	lua_pop(L, 1);
	if (lua_getfield(L, -1, "width")) {
		desc.width = luaL_checknumber(L, -1);
	}
	lua_pop(L, 1);
	if (lua_getfield(L, -1, "height")) {
		desc.height = luaL_checknumber(L, -1);
	}
	lua_pop(L, 1);
	if (lua_getfield(L, -1, "fullscreen")) {
		desc.fullscreen = luaL_checkboolean(L, -1);
	}
	lua_pop(L, 1);
	if (lua_getfield(L, -1, "vsync")) {
		desc.vsync = luaL_checkboolean(L, -1);
	}
	lua_pop(L, 1);
	if (lua_getfield(L, -1, "hidpi")) {
		desc.hidpi = luaL_checkboolean(L, -1);
	}
	lua_pop(L, 1);
	if (lua_getfield(L, -1, "borderless")) {
		desc.borderless = luaL_checkboolean(L, -1);
	}
	lua_pop(L, 1);
	if (lua_getfield(L, -1, "always_on_top")) {
		desc.always_on_top = luaL_checkboolean(L, -1);
	}
	lua_pop(L, 1);

	d_app_run(desc);

	return 0;

}

static int l_app_quit(lua_State* L) {
	d_app_quit();
	return 0;
}

static int l_app_present(lua_State* L) {

	int w = luaL_checknumber(L, 1);
	int h = luaL_checknumber(L, 2);
	luaL_checktable(L, 3);

	d_color* buf = malloc(sizeof(d_color) * w * h);
	int i = 0;

	lua_pushnil(L);

	while (lua_next(L, -2)) {
		lua_pushvalue(L, -2);
		d_color* c = luaL_checkudata(L, -2, "color");
		buf[i++] = *c;
		lua_pop(L, 2);
	}

	lua_pop(L, 1);
	d_app_present(w, h, buf);

	return 0;

}

static int l_app_time(lua_State* L) {
	lua_pushnumber(L, d_app_time());
	return 1;
}

static int l_app_dt(lua_State* L) {
	lua_pushnumber(L, d_app_dt());
	return 1;
}

static int l_app_fps(lua_State* L) {
	lua_pushnumber(L, d_app_fps());
	return 1;
}

static int l_app_key_pressed(lua_State* L) {
	const char* k = luaL_checkstring(L, 1);
	lua_pushboolean(L, d_app_key_pressed(luaL_getenum(L, l_app_key_map, k)));
	return 1;
}

static int l_app_key_rpressed(lua_State* L) {
	const char* k = luaL_checkstring(L, 1);
	lua_pushboolean(L, d_app_key_rpressed(luaL_getenum(L, l_app_key_map, k)));
	return 1;
}

static int l_app_key_released(lua_State* L) {
	const char* k = luaL_checkstring(L, 1);
	lua_pushboolean(L, d_app_key_released(luaL_getenum(L, l_app_key_map, k)));
	return 1;
}

static int l_app_key_down(lua_State* L) {
	const char* k = luaL_checkstring(L, 1);
	lua_pushboolean(L, d_app_key_down(luaL_getenum(L, l_app_key_map, k)));
	return 1;
}

static int l_app_key_mod(lua_State* L) {
	const char* k = luaL_checkstring(L, 1);
	lua_pushboolean(L, d_app_key_mod(luaL_getenum(L, l_app_keymod_map, k)));
	return 1;
}

static int l_app_mouse_pressed(lua_State* L) {
	const char* m = luaL_optlstring(L, 1, "left", NULL);
	lua_pushboolean(L, d_app_mouse_pressed(luaL_getenum(L, l_app_mouse_map, m)));
	return 1;
}

static int l_app_mouse_released(lua_State* L) {
	const char* m = luaL_optlstring(L, 1, "left", NULL);
	lua_pushboolean(L, d_app_mouse_released(luaL_getenum(L, l_app_mouse_map, m)));
	return 1;
}

static int l_app_mouse_down(lua_State* L) {
	const char* m = luaL_optlstring(L, 1, "left", NULL);
	lua_pushboolean(L, d_app_mouse_down(luaL_getenum(L, l_app_mouse_map, m)));
	return 1;
}

static int l_app_mouse_pos(lua_State* L) {
	d_vec2 mpos = d_app_mouse_pos();
	lua_pushudata(L, d_vec2, "vec2", &mpos);
	return 1;
}

static int l_app_mouse_dpos(lua_State* L) {
	d_vec2 dpos = d_app_mouse_dpos();
	lua_pushudata(L, d_vec2, "vec2", &dpos);
	return 1;
}

static int l_app_mouse_moved(lua_State* L) {
	lua_pushboolean(L, d_app_mouse_moved());
	return 1;
}

static int l_app_resized(lua_State* L) {
	lua_pushboolean(L, d_app_resized());
	return 1;
}

static int l_app_scrolled(lua_State* L) {
	lua_pushboolean(L, d_app_scrolled());
	return 1;
}

static int l_app_has_focus(lua_State* L) {
	lua_pushboolean(L, d_app_has_focus());
	return 1;
}

static int l_app_input(lua_State* L) {
	char c = d_app_input();
	if (c) {
		lua_pushlstring(L, &c, 1);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int l_app_width(lua_State* L) {
	lua_pushinteger(L, d_app_width());
	return 1;
}

static int l_app_height(lua_State* L) {
	lua_pushinteger(L, d_app_height());
	return 1;
}

static int l_app_fullscreen(lua_State* L) {
	if (lua_isnoneornil(L, 1)) {
		lua_pushboolean(L, d_app_is_fullscreen());
		return 1;
	} else {
		bool b = luaL_checkboolean(L, 1);
		d_app_set_fullscreen(b);
		return 0;
	}
}

static int l_app_mouse_locked(lua_State* L) {
	if (lua_isnoneornil(L, 1)) {
		lua_pushboolean(L, d_app_is_mouse_locked());
		return 1;
	} else {
		bool b = luaL_checkboolean(L, 1);
		d_app_set_mouse_locked(b);
		return 0;
	}
}

static int l_app_mouse_hidden(lua_State* L) {
	if (lua_isnoneornil(L, 1)) {
		lua_pushboolean(L, d_app_is_mouse_hidden());
		return 1;
	} else {
		bool b = luaL_checkboolean(L, 1);
		d_app_set_mouse_hidden(b);
		return 0;
	}
}

static int l_app_keyboard_shown(lua_State* L) {
	if (lua_isnoneornil(L, 1)) {
		lua_pushboolean(L, d_app_is_keyboard_shown());
		return 1;
	} else {
		bool b = luaL_checkboolean(L, 1);
		d_app_set_keyboard_shown(b);
		return 0;
	}
}

static int l_app_title(lua_State* L) {
	if (lua_isnoneornil(L, 1)) {
		return 0;
		// lua_pushstring(L, d_app_title());
		// return 1;
	} else {
		const char* title = luaL_checkstring(L, 1);
		d_app_set_title(title);
		return 0;
	}
}

void l_app_cleanup(lua_State* L) {
	luaL_unref(L, LUA_REGISTRYINDEX, g.app_init_ref);
	luaL_unref(L, LUA_REGISTRYINDEX, g.app_frame_ref);
	luaL_unref(L, LUA_REGISTRYINDEX, g.app_quit_ref);
	luaL_unref(L, LUA_REGISTRYINDEX, g.app_err_ref);
	luaL_unref(L, LUA_REGISTRYINDEX, g.gfx_shader_ref);
}

static luaL_Reg app_funcs[] = {
	{ "run", l_app_run, },
	{ "quit", l_app_quit, },
	{ "present", l_app_present },
	{ "time", l_app_time, },
	{ "dt", l_app_dt, },
	{ "fps", l_app_fps, },
	{ "width", l_app_width, },
	{ "height", l_app_height, },
	{ "key_pressed", l_app_key_pressed, },
	{ "key_rpressed", l_app_key_rpressed, },
	{ "key_released", l_app_key_released, },
	{ "key_down", l_app_key_down, },
	{ "key_mod", l_app_key_mod },
	{ "mouse_pressed", l_app_mouse_pressed, },
	{ "mouse_released", l_app_mouse_released, },
	{ "mouse_down", l_app_mouse_down, },
	{ "mouse_pos", l_app_mouse_pos, },
	{ "mouse_dpos", l_app_mouse_dpos, },
	{ "mouse_moved", l_app_mouse_moved, },
	{ "resized", l_app_resized, },
	{ "scrolled", l_app_scrolled, },
	{ "input", l_app_input, },
	{ "has_focus", l_app_has_focus, },
	{ "fullscreen", l_app_fullscreen, },
	{ "mouse_locked", l_app_mouse_locked, },
	{ "mouse_hidden", l_app_mouse_hidden, },
	{ "keyboard_shown", l_app_keyboard_shown, },
	{ "title", l_app_title, },
	{ NULL, NULL, },
};

static int l_gfx_init(lua_State* L) {
	d_gfx_desc desc = {0};
	if (!lua_isnoneornil(L, 1)) {
		luaL_checktable(L, 1);
		if (lua_getfield(L, -1, "width")) {
			desc.width = luaL_checknumber(L, -1);
		}
		lua_pop(L, 1);
		if (lua_getfield(L, -1, "height")) {
			desc.height = luaL_checknumber(L, -1);
		}
		lua_pop(L, 1);
		if (lua_getfield(L, -1, "scale")) {
			desc.scale = luaL_checknumber(L, -1);
		}
		lua_pop(L, 1);
		if (lua_getfield(L, -1, "clear_color")) {
			desc.clear_color = *(d_color*)luaL_checkudata(L, -1, "color");
		}
		lua_pop(L, 1);
		if (lua_getfield(L, -1, "depth_test")) {
			desc.depth_test = luaL_checkboolean(L, -1);
		}
		lua_pop(L, 1);
		if (lua_getfield(L, -1, "backface_cull")) {
			desc.backface_cull = luaL_checkboolean(L, -1);
		}
		lua_pop(L, 1);
	}
	d_gfx_init(desc);
	return 0;
}

static int l_gfx_width(lua_State* L) {
	lua_pushinteger(L, d_gfx_width());
	return 1;
}

static int l_gfx_height(lua_State* L) {
	lua_pushinteger(L, d_gfx_height());
	return 1;
}

static int l_gfx_clear(lua_State* L) {
	d_gfx_clear();
	return 0;
}

static int l_gfx_get_pixel(lua_State* L) {
	int x = luaL_checknumber(L, 1);
	int y = luaL_checknumber(L, 2);
	d_color c = d_gfx_get_pixel(x, y);
	lua_pushudata(L, d_vec2, "color", &c);
	return 1;
}

static int l_gfx_blit_bg(lua_State* L) {
	d_blit_bg();
	return 0;
}

static int l_gfx_present(lua_State* L) {
	d_gfx_present();
	return 0;
}

static int l_gfx_mouse_pos(lua_State* L) {
	d_vec2 mpos = d_gfx_mouse_pos();
	lua_pushudata(L, d_vec2, "vec2", &mpos);
	return 1;
}

static int l_gfx_mouse_dpos(lua_State* L) {
	d_vec2 mpos = d_gfx_mouse_dpos();
	lua_pushudata(L, d_vec2, "vec2", &mpos);
	return 1;
}

static int l_gfx_blit_pixel(lua_State* L) {
	int x = luaL_checknumber(L, 1);
	int y = luaL_checknumber(L, 2);
	d_color* c = luaL_optudata(L, 3, "color", &D_WHITE);
	d_blit_pixel(x, y, *c);
	return 0;
}

static int l_gfx_blit_text(lua_State* L) {
	const char* str = luaL_checkstring(L, 1);
	d_vec2* pos = luaL_optudata(L, 2, "vec2", &(d_vec2) { 0, 0 });
	d_color* c = luaL_optudata(L, 3, "color", &D_WHITE);
	bool bold = luaL_optboolean(L, 4, false);
	bool italic = luaL_optboolean(L, 5, false);
	d_blit_text(str, *pos, *c, bold, italic);
	return 0;
}

static int l_gfx_blit_line(lua_State* L) {
	d_vec2* p1 = luaL_checkudata(L, 1, "vec2");
	d_vec2* p2 = luaL_checkudata(L, 2, "vec2");
	d_color* c = luaL_optudata(L, 3, "color", &D_WHITE);
	d_blit_line(*p1, *p2, *c);
	return 0;
}

static int l_gfx_blit_rect(lua_State* L) {
	d_vec2* p1 = luaL_checkudata(L, 1, "vec2");
	d_vec2* p2 = luaL_checkudata(L, 2, "vec2");
	d_color* c = luaL_optudata(L, 3, "color", &D_WHITE);
	d_blit_rect(*p1, *p2, *c);
	return 0;
}

static int l_gfx_blit_circle(lua_State* L) {
	d_vec2* center = luaL_checkudata(L, 1, "vec2");
	float radius = luaL_checknumber(L, 2);
	d_color* c = luaL_optudata(L, 3, "color", &D_WHITE);
	d_blit_circle(*center, radius, *c);
	return 0;
}

static int l_gfx_blit_tri(lua_State* L) {
	d_vec2* p1 = luaL_checkudata(L, 1, "vec2");
	d_vec2* p2 = luaL_checkudata(L, 2, "vec2");
	d_vec2* p3 = luaL_checkudata(L, 3, "vec2");
	d_color* c = luaL_optudata(L, 4, "color", &D_WHITE);
	d_blit_tri(*p1, *p2, *p3, *c);
	return 0;
}

static d_poly l_get_poly(lua_State* L, int pos) {
	d_poly poly = {0};
	luaL_checktable(L, pos);
	int num_verts = luaL_tablelen(L, 1);
	if (num_verts > D_MAX_POLY_VERTS) {
		luaL_error(L, "%d exceeds max polygon verts", num_verts);
		return poly;
	}
	poly.num_verts = num_verts;
	lua_pushnil(L);
	for (int i = 0; i < num_verts; i++) {
		lua_next(L, 1);
		d_vec2* p = luaL_checkudata(L, -1, "vec2");
		poly.verts[i] = *p;
		lua_pop(L, 1);
	}
	lua_pop(L, 1);
	return poly;
}

static int l_gfx_blit_poly(lua_State* L) {
	d_poly poly = l_get_poly(L, 1);
	d_color* c = luaL_optudata(L, 2, "color", &D_WHITE);
	d_blit_poly(poly, *c);
	return 0;
}

static int l_gfx_blit_img(lua_State* L) {
	d_img* img = luaL_checkudata(L, 1, "img");
	d_vec2* pos = luaL_checkudata(L, 2, "vec2");
	d_blit_img(img, *pos);
	return 0;
}

static int l_gfx_draw_pixel(lua_State* L) {
	int x = luaL_checknumber(L, 1);
	int y = luaL_checknumber(L, 2);
	int z = luaL_checknumber(L, 3);
	d_color* c = luaL_optudata(L, 4, "color", &D_WHITE);
	d_draw_pixel(x, y, z, *c);
	return 0;
}

static int l_gfx_draw_line(lua_State* L) {
	d_vec2* p1 = luaL_checkudata(L, 1, "vec2");
	d_vec2* p2 = luaL_checkudata(L, 2, "vec2");
	d_color* c = luaL_optudata(L, 3, "color", &D_WHITE);
	d_draw_line(*p1, *p2, *c);
	return 0;
}

static int l_gfx_draw_rect(lua_State* L) {
	d_vec2* p1 = luaL_checkudata(L, 1, "vec2");
	d_vec2* p2 = luaL_checkudata(L, 2, "vec2");
	d_color* c = luaL_optudata(L, 3, "color", &D_WHITE);
	d_draw_rect(*p1, *p2, *c);
	return 0;
}

static int l_gfx_draw_circle(lua_State* L) {
	float radius = luaL_checknumber(L, 1);
	d_color* c = luaL_optudata(L, 2, "color", &D_WHITE);
	d_draw_circle(radius, *c);
	return 0;
}

static int l_gfx_draw_tri(lua_State* L) {
	d_vec2* p1 = luaL_checkudata(L, 1, "vec2");
	d_vec2* p2 = luaL_checkudata(L, 2, "vec2");
	d_vec2* p3 = luaL_checkudata(L, 3, "vec2");
	d_color* c = luaL_optudata(L, 4, "color", &D_WHITE);
	d_draw_tri(*p1, *p2, *p3, *c);
	return 0;
}

static int l_gfx_draw_poly(lua_State* L) {
	d_poly poly = l_get_poly(L, 1);
	d_color* c = luaL_optudata(L, 2, "color", &D_WHITE);
	d_draw_poly(poly, *c);
	return 0;
}

static int l_gfx_draw_img(lua_State* L) {
	d_img* img = luaL_checkudata(L, 1, "img");
	d_draw_img(img);
	return 0;
}

static int l_gfx_draw_model(lua_State* L) {
	d_model* model = luaL_checkudata(L, 1, "model");
	d_draw_model(model);
	return 0;
}

static int l_gfx_push(lua_State* L) {
	d_transform_push();
	return 0;
}

static int l_gfx_pop(lua_State* L) {
	d_transform_pop();
	return 0;
}

static d_vec2 l_vec2_or_number(lua_State* L) {
	if (lua_gettop(L) == 2) {
		float x = luaL_checknumber(L, 1);
		float y = luaL_checknumber(L, 2);
		return (d_vec2) { x, y };
	} else {
		d_vec2* pos = luaL_checkudata(L, 1, "vec2");
		return *pos;
	}
}

static d_vec3 l_vec3_or_number(lua_State* L) {
	if (lua_gettop(L) == 3) {
		float x = luaL_checknumber(L, 1);
		float y = luaL_checknumber(L, 2);
		float z = luaL_checknumber(L, 3);
		return (d_vec3) { x, y, z };
	} else {
		d_vec3* pos = luaL_checkudata(L, 1, "vec3");
		return *pos;
	}
}

static int l_gfx_pos(lua_State* L) {
	d_vec2 pos = l_vec2_or_number(L);
	d_transform_pos(pos);
	return 0;
}

static int l_gfx_pos3(lua_State* L) {
	d_vec3 pos = l_vec3_or_number(L);
	d_transform_pos3(pos);
	return 0;
}

static int l_gfx_scale(lua_State* L) {
	d_vec2 s = l_vec2_or_number(L);
	d_transform_scale(s);
	return 0;
}

static int l_gfx_scale3(lua_State* L) {
	d_vec3 s = l_vec3_or_number(L);
	d_transform_scale3(s);
	return 0;
}

static int l_gfx_rot(lua_State* L) {
	float r = luaL_checknumber(L, 1);
	d_transform_rot(r);
	return 0;
}

static int l_gfx_rotx(lua_State* L) {
	float r = luaL_checknumber(L, 1);
	d_transform_rot_x(r);
	return 0;
}

static int l_gfx_roty(lua_State* L) {
	float r = luaL_checknumber(L, 1);
	d_transform_rot_y(r);
	return 0;
}

static int l_gfx_rotz(lua_State* L) {
	float r = luaL_checknumber(L, 1);
	d_transform_rot_z(r);
	return 0;
}

static int l_gfx_drawon(lua_State* L) {
	d_img* img = luaL_checkudata(L, 1, "img");
	d_gfx_drawon(img);
	return 0;
}

static int l_gfx_get_canvas(lua_State* L) {
	d_img* img = d_gfx_canvas();
	lua_pushlightuserdata(L, img);
	luaL_setmetatable(L, "img");
	return 1;
}

d_color l_gfx_shader(d_color c) {
	lua_rawgeti(g.lua, LUA_REGISTRYINDEX, g.gfx_shader_ref);
	if (lua_isfunction(g.lua, -1)) {
		lua_pushudata(g.lua, d_color, "color", &c);
		lua_call(g.lua, 1, 1);
		d_color* c = luaL_checkudata(g.lua, -1, "color");
		return *c;
	} else {
		luaL_error(g.lua, "shader func ref not set");
	}
	return d_colori(0, 0, 0, 0);
}

static int l_gfx_set_shader(lua_State* L) {
	if (lua_isnil(L, 1)) {
		d_gfx_set_shader(NULL);
	} else {
		luaL_checktype(L, 1, LUA_TFUNCTION);
		g.gfx_shader_ref = luaL_ref(L, LUA_REGISTRYINDEX);
		d_gfx_set_shader(l_gfx_shader);
	}
	return 0;
}

static int l_img_load(lua_State* L) {
	const char* path = luaL_checkstring(L, 1);
	d_img img = d_img_load(path);
	lua_pushudata(L, d_img, "img", &img);
	return 1;
}

static int l_img_dimension(lua_State* L) {
	d_img* img = luaL_checkudata(L, 1, "img");
	d_vec2 d = { img->width, img->height };
	lua_pushudata(L, d_vec2, "vec2", &d);
	return 1;
}

static int l_img_get(lua_State* L) {
	d_img* img = luaL_checkudata(L, 1, "img");
	int x = luaL_checknumber(L, 2);
	int y = luaL_checknumber(L, 3);
	d_color c = d_img_get(img, x, y);
	lua_pushudata(L, d_color, "color", &c);
	return 1;
}

static int l_img_set(lua_State* L) {
	d_img* img = luaL_checkudata(L, 1, "img");
	int x = luaL_checknumber(L, 2);
	int y = luaL_checknumber(L, 3);
	d_color* c = luaL_checkudata(L, 4, "color");
	d_img_set(img, x, y, *c);
	return 0;
}

static int l_img_fill(lua_State* L) {
	d_img* img = luaL_checkudata(L, 1, "img");
	d_color* c = luaL_checkudata(L, 2, "color");
	d_img_fill(img, *c);
	return 0;
}

static int l_img_save(lua_State* L) {
	d_img* img = luaL_checkudata(L, 1, "img");
	const char* path = luaL_checkstring(L, 2);
	d_img_save(img, path);
	return 0;
}

static int l_img_clone(lua_State* L) {
	d_img* img = luaL_checkudata(L, 1, "img");
	d_img img2 = d_img_clone(img);
	lua_pushudata(L, d_img, "img", &img2);
	return 1;
}

static int l_img_free(lua_State* L) {
	d_img* img = luaL_checkudata(L, 1, "img");
	d_img_free(img);
	return 0;
}

static int l_img_index(lua_State* L) {
	d_img* img = luaL_checkudata(L, 1, "img");
	const char* key = luaL_checkstring(L, 2);
	if (strcmp(key, "width") == 0) {
		lua_pushinteger(L, img->width);
	} else if (strcmp(key, "height") == 0) {
		lua_pushinteger(L, img->height);
	} else if (strcmp(key, "dimension") == 0) {
		lua_pushcfunction(L, l_img_dimension);
	} else if (strcmp(key, "get") == 0) {
		lua_pushcfunction(L, l_img_get);
	} else if (strcmp(key, "set") == 0) {
		lua_pushcfunction(L, l_img_set);
	} else if (strcmp(key, "fill") == 0) {
		lua_pushcfunction(L, l_img_fill);
	} else if (strcmp(key, "save") == 0) {
		lua_pushcfunction(L, l_img_save);
	} else if (strcmp(key, "clone") == 0) {
		lua_pushcfunction(L, l_img_clone);
	} else if (strcmp(key, "free") == 0) {
		lua_pushcfunction(L, l_img_free);
	} else {
		luaL_error(L, "unknown field '%s' on img", key);
		lua_pushnil(L);
	}
	return 1;
}

static luaL_Reg img_meta[] = {
	{ "__index", l_img_index },
	{ "__gc", l_img_free },
	{ NULL, NULL, },
};

static int l_model_load(lua_State* L) {
	const char* path = luaL_checkstring(L, 1);
	d_model model = d_model_load(path);
	lua_pushudata(L, d_model, "model", &model);
	return 1;
}

static int l_model_free(lua_State* L) {
	d_model* model = luaL_checkudata(L, 1, "model");
	d_model_free(model);
	return 0;
}

static int l_model_index(lua_State* L) {
	d_model* model = luaL_checkudata(L, 1, "model");
	const char* key = luaL_checkstring(L, 2);
	if (strcmp(key, "center") == 0) {
		lua_pushudata(L, d_vec3, "vec3", &model->center);
	} else if (strcmp(key, "free") == 0) {
		lua_pushcfunction(L, l_model_free);
	} else {
		luaL_error(L, "unknown field '%s' on model", key);
		lua_pushnil(L);
	}
	return 1;
}

static luaL_Reg model_meta[] = {
	{ "__index", l_model_index },
	{ "__gc", l_model_free },
	{ NULL, NULL, },
};

static luaL_Reg gfx_funcs[] = {
	{ "init", l_gfx_init },
	{ "width", l_gfx_width, },
	{ "height", l_gfx_height, },
	{ "clear", l_gfx_clear },
	{ "present", l_gfx_present },
	{ "mouse_pos", l_gfx_mouse_pos },
	{ "mouse_dpos", l_gfx_mouse_dpos },
	{ "get_pixel", l_gfx_get_pixel },
	{ "blit_bg", l_gfx_blit_bg },
	{ "blit_pixel", l_gfx_blit_pixel },
	{ "blit_text", l_gfx_blit_text },
	{ "blit_line", l_gfx_blit_line },
	{ "blit_rect", l_gfx_blit_rect },
	{ "blit_circle", l_gfx_blit_circle },
	{ "blit_tri", l_gfx_blit_tri },
	{ "blit_poly", l_gfx_blit_poly },
	{ "blit_img", l_gfx_blit_img },
	{ "draw_pixel", l_gfx_draw_pixel },
	{ "draw_tri", l_gfx_draw_tri },
	{ "draw_line", l_gfx_draw_line },
	{ "draw_rect", l_gfx_draw_rect },
	{ "draw_circle", l_gfx_draw_circle },
	{ "draw_tri", l_gfx_draw_tri },
	{ "draw_poly", l_gfx_draw_poly },
	{ "draw_img", l_gfx_draw_img },
	{ "draw_model", l_gfx_draw_model },
	{ "push", l_gfx_push },
	{ "pop", l_gfx_pop },
	{ "pos", l_gfx_pos },
	{ "pos3", l_gfx_pos3 },
	{ "scale", l_gfx_scale },
	{ "scale3", l_gfx_scale3 },
	{ "rot", l_gfx_rot },
	{ "rotx", l_gfx_rotx },
	{ "roty", l_gfx_roty },
	{ "rotz", l_gfx_rotz },
	{ "drawon", l_gfx_drawon },
	{ "get_canvas", l_gfx_get_canvas },
	{ "set_shader", l_gfx_set_shader },
	{ "img_load", l_img_load },
	{ "model_load", l_model_load },
	{ NULL, NULL, },
};

static int l_audio_init(lua_State* L) {
	d_audio_init((d_audio_desc) {0});
	return 0;
}

static int l_audio_dispose(lua_State* L) {
	d_audio_dispose();
	return 0;
}

static int l_audio_play(lua_State* L) {
	d_sound* sound = luaL_checkudata(L, 1, "sound");
	if (lua_isnoneornil(L, 2)) {
		d_play(sound);
	} else {
		d_play_opts opts = {
			.loop = false,
			.paused = false,
			.volume = 1.0,
			.speed = 1.0,
		};
		luaL_checktable(L, 2);
		if (lua_getfield(L, -1, "loop")) {
			opts.loop = luaL_checkboolean(L, -1);
		}
		lua_pop(L, 1);
		if (lua_getfield(L, -1, "paused")) {
			opts.paused = luaL_checkboolean(L, -1);
		}
		lua_pop(L, 1);
		if (lua_getfield(L, -1, "volume")) {
			opts.volume = luaL_checknumber(L, -1);
		}
		lua_pop(L, 1);
		if (lua_getfield(L, -1, "speed")) {
			opts.speed = luaL_checknumber(L, -1);
		}
		lua_pop(L, 1);
		d_play_ex(sound, opts);
	}
	return 1;
}

static int l_sound_load(lua_State* L) {
	const char* path = luaL_checkstring(L, 1);
	d_sound sound = d_sound_load(path);
	lua_pushudata(L, d_sound, "sound", &sound);
	return 1;
}

static int l_sound_duration(lua_State* L) {
	d_sound* sound = luaL_checkudata(L, 1, "sound");
	lua_pushnumber(L, d_sound_duration(sound));
	return 1;
}

static int l_sound_sample(lua_State* L) {
	d_sound* sound = luaL_checkudata(L, 1, "sound");
	float time = luaL_checknumber(L, 2);
	lua_pushnumber(L, d_sound_sample(sound, time));
	return 1;
}

static int l_sound_free(lua_State* L) {
	d_sound* sound = luaL_checkudata(L, 1, "sound");
	d_sound_free(sound);
	return 0;
}

static int l_sound_index(lua_State* L) {
	d_sound* sound = luaL_checkudata(L, 1, "sound");
	const char* key = luaL_checkstring(L, 2);
	if (strcmp(key, "duration") == 0) {
		lua_pushcfunction(L, l_sound_duration);
	} else if (strcmp(key, "sample") == 0) {
		lua_pushcfunction(L, l_sound_sample);
	} else if (strcmp(key, "free") == 0) {
		lua_pushcfunction(L, l_sound_free);
	} else {
		luaL_error(L, "unknown field '%s' on sound", key);
		lua_pushnil(L);
	}
	return 1;
}

static luaL_Reg sound_meta[] = {
	{ "__index", l_sound_index },
	{ "__gc", l_sound_free },
	{ NULL, NULL, },
};

static int l_playback_seek(lua_State* L) {
	d_playback* pb = luaL_checkudata(L, 1, "playback");
	float t = luaL_checknumber(L, 2);
	d_playback_seek(pb, t);
	return 0;
}

static int l_playback_seek_by(lua_State* L) {
	d_playback* pb = luaL_checkudata(L, 1, "playback");
	float t = luaL_checknumber(L, 2);
	d_playback_seek_by(pb, t);
	return 0;
}

static int l_playback_index(lua_State* L) {
	d_playback* pb = luaL_checkudata(L, 1, "playback");
	const char* key = luaL_checkstring(L, 2);
	if (strcmp(key, "paused") == 0) {
		lua_pushboolean(L, pb->paused);
	} else if (strcmp(key, "done") == 0) {
		lua_pushboolean(L, pb->loop);
	} else if (strcmp(key, "loop") == 0) {
		lua_pushboolean(L, pb->done);
	} else if (strcmp(key, "volume") == 0) {
		lua_pushnumber(L, pb->volume);
	} else if (strcmp(key, "speed") == 0) {
		lua_pushnumber(L, pb->speed);
	} else if (strcmp(key, "time") == 0) {
		lua_pushnumber(L, d_playback_time(pb));
	} else if (strcmp(key, "seek") == 0) {
		lua_pushcfunction(L, l_playback_seek);
	} else if (strcmp(key, "seek_by") == 0) {
		lua_pushcfunction(L, l_playback_seek_by);
	} else if (strcmp(key, "src") == 0) {
		lua_pushlightuserdata(L, pb->src);
		luaL_setmetatable(L, "sound");
	} else {
		luaL_error(L, "unknown field '%s' on playback", key);
		lua_pushnil(L);
	}
	return 1;
}

static int l_playback_newindex(lua_State* L) {
	d_playback* pb = luaL_checkudata(L, 1, "playback");
	const char* key = luaL_checkstring(L, 2);
	if (strcmp(key, "paused") == 0) {
		bool p = luaL_checkboolean(L, 3);
		pb->paused = p;
	} else if (strcmp(key, "loop") == 0) {
		bool l = luaL_checkboolean(L, 3);
		pb->loop = l;
	} else if (strcmp(key, "volume") == 0) {
		float v = luaL_checknumber(L, 3);
		pb->volume = v;
	} else if (strcmp(key, "speed") == 0) {
		float p = luaL_checknumber(L, 3);
		pb->speed = p;
	} else if (strcmp(key, "time") == 0) {
		float t = luaL_checknumber(L, 3);
		d_playback_seek(pb, t);
	}
	return 0;
}

static luaL_Reg playback_meta[] = {
	{ "__index", l_playback_index },
	{ "__newindex", l_playback_newindex },
	{ NULL, NULL, },
};

static luaL_Reg audio_funcs[] = {
	{ "init", l_audio_init, },
	{ "dispose", l_audio_dispose, },
	{ "play", l_audio_play, },
	{ "sound_load", l_sound_load, },
	{ NULL, NULL, },
};

static int l_fs_read_text(lua_State* L) {
	const char* path = luaL_checkstring(L, 1);
	size_t size;
	char* text = d_read_text(path, &size);
	if (text) {
		lua_pushlstring(L, text, size);
		free(text);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int l_fs_write_text(lua_State* L) {
	const char* path = luaL_checkstring(L, 1);
	size_t size;
	const char* content = luaL_checklstring(L, 2, &size);
	d_write_text(path, content, size);
	return 0;
}

static int l_fs_read_dir(lua_State* L) {
	const char* path = luaL_checkstring(L, 1);
	DIR* dir = opendir(path);
	if (dir == NULL) {
		return luaL_error(L, "cannot open directory: %s", path);
	}
	lua_newtable(L);
	int i = 1;
	struct dirent* entry;
	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
			continue;
		lua_pushstring(L, entry->d_name);
		lua_rawseti(L, -2, i++);
	}
	closedir(dir);
	return 1;
}

static int l_fs_is_dir(lua_State* L) {
	const char* path = luaL_checkstring(L, 1);
	lua_pushboolean(L, d_is_dir(path));
	return 1;
}

static int l_fs_is_file(lua_State* L) {
	const char* path = luaL_checkstring(L, 1);
	lua_pushboolean(L, d_is_file(path));
	return 1;
}

static int l_fs_res_dir(lua_State* L) {
	char* dir = d_res_dir();
	if (dir) {
		lua_pushstring(L, dir);
		free(dir);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int l_fs_data_dir(lua_State* L) {
	char* dir = d_data_dir();
	if (dir) {
		lua_pushstring(L, dir);
		free(dir);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int l_fs_basename(lua_State* L) {
	const char* path = luaL_checkstring(L, 1);
	char* name = d_basename(path);
	lua_pushstring(L, name);
	free(name);
	return 1;
}

static int l_fs_extname(lua_State* L) {
	const char* path = luaL_checkstring(L, 1);
	char* name = d_extname(path);
	if (name) {
		lua_pushstring(L, name);
		free(name);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static luaL_Reg fs_funcs[] = {
	{ "read_file", l_fs_read_text, },
	{ "write_file", l_fs_write_text, },
	{ "read_dir", l_fs_read_dir, },
	{ "is_dir", l_fs_is_dir, },
	{ "is_file", l_fs_is_file, },
	{ "extname", l_fs_extname, },
	{ "basename", l_fs_basename, },
	{ "res_dir", l_fs_res_dir, },
	{ "data_dir", l_fs_data_dir, },
	{ NULL, NULL, },
};

static int l_col_pt_pt(lua_State* L) {
	d_vec2* p1 = luaL_checkudata(L, 1, "vec2");
	d_vec2* p2 = luaL_checkudata(L, 2, "vec2");
	lua_pushboolean(L, d_col_pt_pt(*p1, *p2));
	return 1;
}

static int l_col_pt_line(lua_State* L) {
	d_vec2* pt = luaL_checkudata(L, 1, "vec2");
	d_vec2* lp1 = luaL_checkudata(L, 2, "vec2");
	d_vec2* lp2 = luaL_checkudata(L, 3, "vec2");
	lua_pushboolean(L, d_col_pt_line(*pt, (d_line2) { *lp1, *lp2 }));
	return 1;
}

static int l_col_pt_rect(lua_State* L) {
	d_vec2* pt = luaL_checkudata(L, 1, "vec2");
	d_vec2* rp1 = luaL_checkudata(L, 2, "vec2");
	d_vec2* rp2 = luaL_checkudata(L, 3, "vec2");
	lua_pushboolean(L, d_col_pt_rect(*pt, (d_rect) { *rp1, *rp2 }));
	return 1;
}

static int l_col_pt_circle(lua_State* L) {
	d_vec2* pt = luaL_checkudata(L, 1, "vec2");
	d_vec2* c = luaL_checkudata(L, 2, "vec2");
	float r = luaL_checknumber(L, 3);
	lua_pushboolean(L, d_col_pt_circle(*pt, (d_circle) { *c, r }));
	return 1;
}

static int l_col_pt_poly(lua_State* L) {
	d_vec2* pt = luaL_checkudata(L, 1, "vec2");
	d_poly poly = l_get_poly(L, 2);
	lua_pushboolean(L, d_col_pt_poly(*pt, poly));
	return 1;
}

static int l_col_line_line(lua_State* L) {
	d_vec2* l1p1 = luaL_checkudata(L, 1, "vec2");
	d_vec2* l1p2 = luaL_checkudata(L, 2, "vec2");
	d_vec2* l2p1 = luaL_checkudata(L, 3, "vec2");
	d_vec2* l2p2 = luaL_checkudata(L, 4, "vec2");
	d_line2 l1 = { *l1p1, *l1p2 };
	d_line2 l2 = { *l2p1, *l2p2 };
	lua_pushboolean(L, d_col_line_line(l1, l2));
	return 1;
}

static int l_col_line_rect(lua_State* L) {
	d_vec2* lp1 = luaL_checkudata(L, 1, "vec2");
	d_vec2* lp2 = luaL_checkudata(L, 2, "vec2");
	d_vec2* rp1 = luaL_checkudata(L, 3, "vec2");
	d_vec2* rp2 = luaL_checkudata(L, 4, "vec2");
	d_line2 l = { *lp1, *lp2 };
	d_rect r = { *rp1, *rp2 };
	lua_pushboolean(L, d_col_line_rect(l, r));
	return 1;
}

static int l_col_line_circle(lua_State* L) {
	d_vec2* lp1 = luaL_checkudata(L, 1, "vec2");
	d_vec2* lp2 = luaL_checkudata(L, 2, "vec2");
	d_vec2* c = luaL_checkudata(L, 2, "vec2");
	float r = luaL_checknumber(L, 3);
	d_line2 l = { *lp1, *lp2 };
	d_circle circle = { *c, r };
	lua_pushboolean(L, d_col_line_circle(l, circle));
	return 1;
}

static int l_col_line_poly(lua_State* L) {
	d_vec2* lp1 = luaL_checkudata(L, 1, "vec2");
	d_vec2* lp2 = luaL_checkudata(L, 2, "vec2");
	d_poly poly = l_get_poly(L, 3);
	d_line2 l = { *lp1, *lp2 };
	lua_pushboolean(L, d_col_line_poly(l, poly));
	return 1;
}

static int l_col_circle_circle(lua_State* L) {
	d_vec2* p1 = luaL_checkudata(L, 1, "vec2");
	float r1 = luaL_checknumber(L, 2);
	d_vec2* p2 = luaL_checkudata(L, 3, "vec2");
	float r2 = luaL_checknumber(L, 4);
	d_circle c1 = { *p1, r1 };
	d_circle c2 = { *p2, r2 };
	lua_pushboolean(L, d_col_circle_circle(c1, c2));
	return 1;
}

static int l_col_circle_rect(lua_State* L) {
	d_vec2* p = luaL_checkudata(L, 1, "vec2");
	float r = luaL_checknumber(L, 2);
	d_vec2* rp1 = luaL_checkudata(L, 3, "vec2");
	d_vec2* rp2 = luaL_checkudata(L, 4, "vec2");
	d_circle circle = { *p, r };
	d_rect rect = { *rp1, *rp2 };
	lua_pushboolean(L, d_col_circle_rect(circle, rect));
	return 1;
}

static int l_col_circle_poly(lua_State* L) {
	d_vec2* p = luaL_checkudata(L, 1, "vec2");
	float r = luaL_checknumber(L, 2);
	d_poly poly = l_get_poly(L, 3);
	d_circle circle = { *p, r };
	lua_pushboolean(L, d_col_circle_poly(circle, poly));
	return 1;
}

static int l_col_rect_rect(lua_State* L) {
	d_vec2* r1p1 = luaL_checkudata(L, 1, "vec2");
	d_vec2* r1p2 = luaL_checkudata(L, 2, "vec2");
	d_vec2* r2p1 = luaL_checkudata(L, 3, "vec2");
	d_vec2* r2p2 = luaL_checkudata(L, 4, "vec2");
	d_rect r1 = { *r1p1, *r1p2 };
	d_rect r2 = { *r2p1, *r2p2 };
	lua_pushboolean(L, d_col_rect_rect(r1, r2));
	return 1;
}

static int l_col_rect_poly(lua_State* L) {
	d_vec2* rp1 = luaL_checkudata(L, 1, "vec2");
	d_vec2* rp2 = luaL_checkudata(L, 2, "vec2");
	d_poly poly = l_get_poly(L, 3);
	d_rect r = { *rp1, *rp2 };
	lua_pushboolean(L, d_col_rect_poly(r, poly));
	return 1;
}

static int l_col_poly_poly(lua_State* L) {
	d_poly p1 = l_get_poly(L, 1);
	d_poly p2 = l_get_poly(L, 2);
	lua_pushboolean(L, d_col_poly_poly(p1, p2));
	return 1;
}

static int l_col_sat(lua_State* L) {
	d_poly p1 = l_get_poly(L, 1);
	d_poly p2 = l_get_poly(L, 2);
	d_vec2 dis = { 0, 0 };
	bool res = d_col_sat(p1, p2, &dis);
	if (res) {
		lua_pushudata(L, d_vec2, "vec2", &dis);
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static luaL_Reg col_funcs[] = {
	{ "pt_pt", l_col_pt_pt, },
	{ "pt_line", l_col_pt_line, },
	{ "pt_rect", l_col_pt_rect, },
	{ "pt_circle", l_col_pt_circle, },
	{ "pt_poly", l_col_pt_poly, },
	{ "line_line", l_col_line_line, },
	{ "line_rect", l_col_line_rect, },
	{ "line_circle", l_col_line_circle, },
	{ "line_poly", l_col_line_poly, },
	{ "circle_circle", l_col_circle_circle, },
	{ "circle_rect", l_col_circle_rect, },
	{ "circle_poly", l_col_circle_poly, },
	{ "rect_rect", l_col_rect_rect, },
	{ "rect_poly", l_col_rect_poly, },
	{ "poly_poly", l_col_poly_poly, },
	{ "sat", l_col_sat, },
	{ NULL, NULL, },
};

static int l_tween_update(lua_State* L) {
	luaL_checktable(L, 1);
	float dt = luaL_checknumber(L, 2);
	bool done = luaL_getfieldboolean(L, 1, "done");
	bool paused = luaL_getfieldboolean(L, 1, "paused");
	if (done || paused) return 0;
	float elapsed = luaL_getfieldnumber(L, 1, "elapsed");
	float duration = luaL_getfieldnumber(L, 1, "duration");
	elapsed += dt;
	float t = fminf(elapsed / duration, 1.0);
	lua_getfield(L, 1, "func");
	lua_pushnumber(L, t);
	lua_call(L, 1, 1);
	float t2 = luaL_checknumber(L, -1);
	lua_getfield(L, 1, "action");
	switch (luaL_getfieldtype(L, 1, "val")) {
		case LUA_TNUMBER: {
			float from = luaL_getfieldnumber(L, 1, "from");
			float to = luaL_getfieldnumber(L, 1, "to");
			float val = d_tween_lerp(from, to, t2);;
			luaL_setfieldnumber(L, 1, "val", val);
			break;
		}
		case LUA_TUSERDATA: {
			lua_getfield(L, 1, "val");
			const char* ty = luaL_udatatype(L, -1);
			lua_pop(L, 1);
			if (strcmp(ty, "vec2") == 0) {
				d_vec2* from = luaL_getfieldudata(L, 1, "from", "vec2");
				d_vec2* to = luaL_getfieldudata(L, 1, "to", "vec2");
				float x = d_tween_lerp(from->x, to->x, t2);
				float y = d_tween_lerp(from->y, to->y, t2);
				d_vec2 p = { x, y };
				luaL_setfieldudata(L, 1, "val", d_vec2, "vec2", p);
			} else if (strcmp(ty, "vec3")) {
				d_vec3* from = luaL_getfieldudata(L, 1, "from", "vec3");
				d_vec3* to = luaL_getfieldudata(L, 1, "to", "vec3");
				float x = d_tween_lerp(from->x, to->x, t2);
				float y = d_tween_lerp(from->y, to->y, t2);
				float z = d_tween_lerp(from->z, to->z, t2);
				d_vec3 p = { x, y, z };
				luaL_setfieldudata(L, 1, "val", d_vec3, "vec3", p);
			} else if (strcmp(ty, "color")) {
				d_color* from = luaL_getfieldudata(L, 1, "from", "color");
				d_color* to = luaL_getfieldudata(L, 1, "to", "color");
				float r = d_tween_lerp(from->r, to->r, t2);
				float g = d_tween_lerp(from->g, to->g, t2);
				float b = d_tween_lerp(from->b, to->b, t2);
				float a = d_tween_lerp(from->a, to->a, t2);
				d_color c = { r, g, b, a };
				luaL_setfieldudata(L, 1, "val", d_color, "color", c);
			} else {
				luaL_error(L, "unsupported tween type \"%s\"", ty);
			}
			break;
		}
		default:
			luaL_error(L, "unsupported tween type");
			break;
	}
	if (t >= 1.0f) {
		elapsed = duration;
		done = true;
	}
	luaL_setfieldnumber(L, 1, "elapsed", elapsed);
	luaL_setfieldboolean(L, 1, "done", done);
	lua_pushvalue(L, 1);
	lua_call(L, 1, 0);
	return 0;
}

static int l_tween_new(lua_State* L) {
	luaL_checkfunction(L, 4);
	lua_newtable(L);
	lua_pushvalue(L, 1);
	lua_setfield(L, -2, "from");
	lua_pushvalue(L, 1);
	lua_setfield(L, -2, "val");
	lua_pushvalue(L, 2);
	lua_setfield(L, -2, "to");
	lua_pushvalue(L, 3);
	lua_setfield(L, -2, "duration");
	lua_pushvalue(L, 4);
	lua_setfield(L, -2, "func");
	lua_pushvalue(L, 5);
	lua_setfield(L, -2, "action");
	lua_pushvalue(L, 6);
	lua_setfield(L, -2, "name");
	lua_pushnumber(L, 0);
	lua_setfield(L, -2, "elapsed");
	lua_pushboolean(L, false);
	lua_setfield(L, -2, "paused");
	lua_pushboolean(L, false);
	lua_setfield(L, -2, "done");
	lua_pushcfunction(L, l_tween_update);
	lua_setfield(L, -2, "update");
	return 1;
}

static int l_tween_manager_add(lua_State* L) {

	lua_pushcfunction(L, l_tween_new);

	for (int i = 2; i <= 7; i++) {
		lua_pushvalue(L, i);
	}

	lua_call(L, 6, 1);

	const char* name = luaL_getfieldstringopt(L, -1, "name");

	lua_getfield(L, 1, "tweens");
	int len = lua_rawlen(L, -1);

	// cancel previous tween with the same name
	if (name) {
		const char* name = luaL_checkstring(L, 7);
		for (int i = len; i >= 1; i--) {
			lua_geti(L, -1, i);
			const char* name2 = luaL_getfieldstring(L, -1, "name");
			if (strcmp(name, name2) == 0) {
				luaL_rmi(L, -2, i);
				len -= 1;
			}
			lua_pop(L, 1);
		}
	}

	lua_pushvalue(L, -2);
	lua_seti(L, -2, len + 1);
	lua_pop(L, 1);

	return 1;

}

static int l_tween_manager_update(lua_State* L) {

	float dt = luaL_checknumber(L, 2);
	lua_getfield(L, 1, "tweens");
	int len = lua_rawlen(L, -1);

	for (int i = len; i >= 1; --i) {

		lua_geti(L, -1, i);

		lua_pushcfunction(L, l_tween_update);
		lua_pushvalue(L, -2);
		lua_pushnumber(L, dt);
		lua_call(L, 2, 0);

		bool done = luaL_getfieldboolean(L, -1, "done");

		if (done) {
			luaL_rmi(L, 3, i);
		}

		lua_pop(L, 1);

	}

	return 0;

}

static int l_tween_manager(lua_State* L) {
	lua_newtable(L);
	lua_newtable(L);
	lua_setfield(L, -2, "tweens");
	lua_pushcfunction(L, l_tween_manager_add);
	lua_setfield(L, -2, "add");
	lua_pushcfunction(L, l_tween_manager_update);
	lua_setfield(L, -2, "update");
	return 1;
}

static luaL_Reg tween_funcs[] = {
	{ "manager", l_tween_manager, },
	{ "new", l_tween_new, },
	{ NULL, NULL, },
};

#define BIND_EASE_FUNC(name) \
	static int l_ease_##name(lua_State* L) { \
		lua_pushnumber(L, d_ease_##name(luaL_checknumber(L, 1))); \
		return 1; \
	}

BIND_EASE_FUNC(linear)
BIND_EASE_FUNC(in_sine)
BIND_EASE_FUNC(out_sine)
BIND_EASE_FUNC(in_out_sine)
BIND_EASE_FUNC(in_quad)
BIND_EASE_FUNC(out_quad)
BIND_EASE_FUNC(in_out_quad)
BIND_EASE_FUNC(in_cubic)
BIND_EASE_FUNC(out_cubic)
BIND_EASE_FUNC(in_out_cubic)
BIND_EASE_FUNC(in_quart)
BIND_EASE_FUNC(out_quart)
BIND_EASE_FUNC(in_out_quart)
BIND_EASE_FUNC(in_quint)
BIND_EASE_FUNC(out_quint)
BIND_EASE_FUNC(in_out_quint)
BIND_EASE_FUNC(in_etpo)
BIND_EASE_FUNC(out_etpo)
BIND_EASE_FUNC(in_out_etpo)
BIND_EASE_FUNC(in_circ)
BIND_EASE_FUNC(out_circ)
BIND_EASE_FUNC(in_out_circ)
BIND_EASE_FUNC(in_back)
BIND_EASE_FUNC(out_back)
BIND_EASE_FUNC(in_out_back)
BIND_EASE_FUNC(in_elastic)
BIND_EASE_FUNC(out_elastic)
BIND_EASE_FUNC(in_out_elastic)
BIND_EASE_FUNC(in_bounce)
BIND_EASE_FUNC(out_bounce)
BIND_EASE_FUNC(in_out_bounce)

static luaL_Reg ease_funcs[] = {
	{ "linear", l_ease_linear, },
	{ "in_sine", l_ease_in_sine, },
	{ "out_sine", l_ease_out_sine, },
	{ "in_out_sine", l_ease_in_out_sine, },
	{ "in_quad", l_ease_in_quad, },
	{ "out_quad", l_ease_out_quad, },
	{ "in_out_quad", l_ease_in_out_quad, },
	{ "in_cubic", l_ease_in_cubic, },
	{ "out_cubic", l_ease_out_cubic, },
	{ "in_out_cubic", l_ease_in_out_cubic, },
	{ "in_quart", l_ease_in_quart, },
	{ "out_quart", l_ease_out_quart, },
	{ "in_out_quart", l_ease_in_out_quart, },
	{ "in_quint", l_ease_in_quint, },
	{ "out_quint", l_ease_out_quint, },
	{ "in_out_quint", l_ease_in_out_quint, },
	{ "in_etpo", l_ease_in_etpo, },
	{ "out_etpo", l_ease_out_etpo, },
	{ "in_out_etpo", l_ease_in_out_etpo, },
	{ "in_circ", l_ease_in_circ, },
	{ "out_circ", l_ease_out_circ, },
	{ "in_out_circ", l_ease_in_out_circ, },
	{ "in_back", l_ease_in_back, },
	{ "out_back", l_ease_out_back, },
	{ "in_out_back", l_ease_in_out_back, },
	{ "in_elastic", l_ease_in_elastic, },
	{ "out_elastic", l_ease_out_elastic, },
	{ "in_out_elastic", l_ease_in_out_elastic, },
	{ "in_bounce", l_ease_in_bounce, },
	{ "out_bounce", l_ease_out_bounce, },
	{ "in_out_bounce", l_ease_in_out_bounce, },
	{ NULL, NULL, },
};

static int l_vec2_len(lua_State* L) {
	d_vec2* p = luaL_checkudata(L, 1, "vec2");
	float l = d_vec2_len(*p);
	lua_pushnumber(L, l);
	return 1;
}

static int l_vec2_dist(lua_State* L) {
	d_vec2* p1 = luaL_checkudata(L, 1, "vec2");
	d_vec2* p2 = luaL_checkudata(L, 2, "vec2");
	float d = d_vec2_dist(*p1, *p2);
	lua_pushnumber(L, d);
	return 1;
}

static int l_vec2_angle(lua_State* L) {
	d_vec2* p1 = luaL_checkudata(L, 1, "vec2");
	d_vec2* p2 = luaL_checkudata(L, 2, "vec2");
	float d = d_vec2_angle(*p1, *p2);
	lua_pushnumber(L, d);
	return 1;
}

static int l_vec2_unit(lua_State* L) {
	d_vec2* p1 = luaL_checkudata(L, 1, "vec2");
	d_vec2 p2 = d_vec2_unit(*p1);
	lua_pushudata(L, d_vec2, "vec2", &p2);
	return 1;
}

static int l_vec2_normal(lua_State* L) {
	d_vec2* p1 = luaL_checkudata(L, 1, "vec2");
	d_vec2 p2 = d_vec2_normal(*p1);
	lua_pushudata(L, d_vec2, "vec2", &p2);
	return 1;
}

static int l_vec2_clone(lua_State* L) {
	d_vec2* p = luaL_checkudata(L, 1, "vec2");
	d_vec2 p2 = *p;
	lua_pushudata(L, d_vec2, "vec2", &p2);
	return 1;
}

static int l_vec2_index(lua_State* L) {
	d_vec2* p = luaL_checkudata(L, 1, "vec2");
	const char* key = luaL_checkstring(L, 2);
	if (strcmp(key, "x") == 0) {
		lua_pushnumber(L, p->x);
	} else if (strcmp(key, "y") == 0) {
		lua_pushnumber(L, p->y);
	} else if (strcmp(key, "unit") == 0) {
		lua_pushcfunction(L, l_vec2_unit);
	} else if (strcmp(key, "len") == 0) {
		lua_pushcfunction(L, l_vec2_len);
	} else if (strcmp(key, "dist") == 0) {
		lua_pushcfunction(L, l_vec2_dist);
	} else if (strcmp(key, "normal") == 0) {
		lua_pushcfunction(L, l_vec2_normal);
	} else if (strcmp(key, "angle") == 0) {
		lua_pushcfunction(L, l_vec2_angle);
	} else if (strcmp(key, "clone") == 0) {
		lua_pushcfunction(L, l_vec2_clone);
	} else {
		luaL_error(L, "unknown field '%s' on vec2", key);
		lua_pushnil(L);
	}
	return 1;
}

static int l_vec2_newindex(lua_State* L) {
	d_vec2* p = luaL_checkudata(L, 1, "vec2");
	const char* key = luaL_checkstring(L, 2);
	float value = luaL_checknumber(L, 3);
	if (strcmp(key, "x") == 0) {
		p->x = value;
	} else if (strcmp(key, "y") == 0) {
		p->y = value;
	}
	return 0;
}

static int l_vec2_tostring(lua_State* L) {
	d_vec2* p = luaL_checkudata(L, 1, "vec2");
	char buf[64];
	snprintf(buf, sizeof(buf), "vec2(%.02f, %.02f)", p->x, p->y);
	lua_pushstring(L, buf);
	return 1;
}

static int l_vec2_add(lua_State* L) {
	d_vec2* p1 = luaL_checkudata(L, 1, "vec2");
	d_vec2* p2 = luaL_checkudata(L, 2, "vec2");
	d_vec2 p = d_vec2_add(*p1, *p2);
	lua_pushudata(L, d_vec2, "vec2", &p);
	return 1;
}

static int l_vec2_sub(lua_State* L) {
	d_vec2* p1 = luaL_checkudata(L, 1, "vec2");
	d_vec2* p2 = luaL_checkudata(L, 2, "vec2");
	d_vec2 p = d_vec2_sub(*p1, *p2);
	lua_pushudata(L, d_vec2, "vec2", &p);
	return 1;
}

static int l_vec2_mul(lua_State* L) {
	d_vec2* p1 = luaL_checkudata(L, 1, "vec2");
	int type = lua_type(L, 2);
	if (type == LUA_TNUMBER) {
		float s = luaL_checknumber(L, 2);
		d_vec2 p = { p1->x * s, p1->y * s };
		lua_pushudata(L, d_vec2, "vec2", &p);
	} else if (type == LUA_TUSERDATA) {
		d_vec2* p2 = luaL_checkudata(L, 2, "vec2");
		d_vec2 p = { p1->x * p2->x, p1->y * p2->y };
		lua_pushudata(L, d_vec2, "vec2", &p);
	} else {
		luaL_error(L, "cannot multiply vec2 with %s", lua_typename(L, type));
		lua_pushnil(L);
	}
	return 1;
}

static int l_vec2_div(lua_State* L) {
	d_vec2* p1 = luaL_checkudata(L, 1, "vec2");
	float s = luaL_checknumber(L, 2);
	d_vec2 p = { p1->x / s, p1->y / s };
	lua_pushudata(L, d_vec2, "vec2", &p);
	return 1;
}

static int l_vec2_eq(lua_State* L) {
	d_vec2* p1 = luaL_checkudata(L, 1, "vec2");
	d_vec2* p2 = luaL_checkudata(L, 2, "vec2");
	lua_pushboolean(L, d_vec2_eq(*p1, *p2));
	return 1;
}

static int l_vec2_unm(lua_State* L) {
	d_vec2* p1 = luaL_checkudata(L, 1, "vec2");
	d_vec2 p2 = { -p1->x, -p1->y };
	lua_pushudata(L, d_vec2, "vec2", &p2);
	return 1;
}

static luaL_Reg vec2_meta[] = {
	{ "__index", l_vec2_index },
	{ "__newindex", l_vec2_newindex },
	{ "__tostring", l_vec2_tostring },
	{ "__add", l_vec2_add },
	{ "__sub", l_vec2_sub },
	{ "__mul", l_vec2_mul },
	{ "__div", l_vec2_div },
	{ "__len", l_vec2_len },
	{ "__eq", l_vec2_eq },
	{ "__unm", l_vec2_unm },
	{ NULL, NULL, },
};

static int l_vec3_len(lua_State* L) {
	d_vec3* p = luaL_checkudata(L, 1, "vec3");
	float l = d_vec3_len(*p);
	lua_pushnumber(L, l);
	return 1;
}

static int l_vec3_clone(lua_State* L) {
	d_vec3* p = luaL_checkudata(L, 1, "vec3");
	d_vec3 p2 = *p;
	lua_pushudata(L, d_vec3, "vec3", &p2);
	return 1;
}

static int l_vec3_index(lua_State* L) {
	d_vec3* p = luaL_checkudata(L, 1, "vec3");
	const char* key = luaL_checkstring(L, 2);
	if (strcmp(key, "x") == 0) {
		lua_pushnumber(L, p->x);
	} else if (strcmp(key, "y") == 0) {
		lua_pushnumber(L, p->y);
	} else if (strcmp(key, "z") == 0) {
		lua_pushnumber(L, p->z);
	} else if (strcmp(key, "len") == 0) {
		lua_pushcfunction(L, l_vec3_len);
	} else if (strcmp(key, "clone") == 0) {
		lua_pushcfunction(L, l_vec3_clone);
	} else {
		luaL_error(L, "unknown field '%s' on vec3", key);
		lua_pushnil(L);
	}
	return 1;
}

static int l_vec3_newindex(lua_State* L) {
	d_vec3* p = luaL_checkudata(L, 1, "vec3");
	const char* key = luaL_checkstring(L, 2);
	float value = luaL_checknumber(L, 3);
	if (strcmp(key, "x") == 0) {
		p->x = value;
	} else if (strcmp(key, "y") == 0) {
		p->y = value;
	} else if (strcmp(key, "z") == 0) {
		p->z = value;
	}
	return 0;
}

static int l_vec3_tostring(lua_State* L) {
	d_vec3* p = luaL_checkudata(L, 1, "vec3");
	char buf[64];
	snprintf(buf, sizeof(buf), "vec3(%.02f, %.02f, %.02f)", p->x, p->y, p->z);
	lua_pushstring(L, buf);
	return 1;
}

static int l_vec3_add(lua_State* L) {
	d_vec3* p1 = luaL_checkudata(L, 1, "vec3");
	d_vec3* p2 = luaL_checkudata(L, 2, "vec3");
	d_vec3 p = d_vec3_add(*p1, *p2);
	lua_pushudata(L, d_vec3, "vec3", &p);
	return 1;
}

static int l_vec3_sub(lua_State* L) {
	d_vec3* p1 = luaL_checkudata(L, 1, "vec3");
	d_vec3* p2 = luaL_checkudata(L, 2, "vec3");
	d_vec3 p = d_vec3_sub(*p1, *p2);
	lua_pushudata(L, d_vec3, "vec3", &p);
	return 1;
}

static int l_vec3_mul(lua_State* L) {
	d_vec3* p1 = luaL_checkudata(L, 1, "vec3");
	int type = lua_type(L, 2);
	if (type == LUA_TNUMBER) {
		float s = luaL_checknumber(L, 2);
		d_vec3 p = { p1->x * s, p1->y * s, p1->z * s };
		lua_pushudata(L, d_vec3, "vec3", &p);
	} else if (type == LUA_TUSERDATA) {
		d_vec3* p2 = luaL_checkudata(L, 2, "vec3");
		d_vec3 p = { p1->x * p1->x, p1->y * p2->y, p1->z * p2->z };
		lua_pushudata(L, d_vec3, "vec3", &p);
	} else {
		luaL_error(L, "cannot multiply vec3 with %s", lua_typename(L, type));
		lua_pushnil(L);
	}
	return 1;
}

static int l_vec3_div(lua_State* L) {
	d_vec3* p1 = luaL_checkudata(L, 1, "vec3");
	float s = luaL_checknumber(L, 2);
	d_vec3 p = { p1->x / s, p1->y / s, p1->z / s };
	lua_pushudata(L, d_vec3, "vec3", &p);
	return 1;
}

static int l_vec3_eq(lua_State* L) {
	d_vec3* p1 = luaL_checkudata(L, 1, "vec3");
	d_vec3* p2 = luaL_checkudata(L, 2, "vec3");
	lua_pushboolean(L, d_vec3_eq(*p1, *p2));
	return 1;
}

static int l_vec3_unm(lua_State* L) {
	d_vec3* p = luaL_checkudata(L, 1, "vec3");
	d_vec3 p2 = { -p->x, -p->y, -p->z };
	lua_pushudata(L, d_vec3, "vec3", &p2);
	return 1;
}

static luaL_Reg vec3_meta[] = {
	{ "__index", l_vec3_index },
	{ "__newindex", l_vec3_newindex },
	{ "__tostring", l_vec3_tostring },
	{ "__add", l_vec3_add },
	{ "__sub", l_vec3_sub },
	{ "__mul", l_vec3_mul },
	{ "__div", l_vec3_div },
	{ "__len", l_vec3_len },
	{ "__eq", l_vec3_eq },
	{ "__unm", l_vec3_unm },
	{ NULL, NULL, },
};

static int l_color_darken(lua_State* L) {
	d_color* c1 = luaL_checkudata(L, 1, "color");
	int s = luaL_checknumber(L, 2);
	d_color c2 = d_color_darken(*c1, s);
	lua_pushudata(L, d_color, "color", &c2);
	return 1;
}

static int l_color_lighten(lua_State* L) {
	d_color* c1 = luaL_checkudata(L, 1, "color");
	int s = luaL_checknumber(L, 2);
	d_color c2 = d_color_lighten(*c1, s);
	lua_pushudata(L, d_color, "color", &c2);
	return 1;
}

static int l_color_mix(lua_State* L) {
	d_color* c1 = luaL_checkudata(L, 1, "color");
	d_color* c2 = luaL_checkudata(L, 2, "color");
	d_color c = d_color_mix(*c1, *c2);
	lua_pushudata(L, d_color, "color", &c);
	return 1;
}

static int l_color_index(lua_State* L) {
	d_color* c = luaL_checkudata(L, 1, "color");
	const char* key = luaL_checkstring(L, 2);
	if (strcmp(key, "r") == 0) {
		lua_pushinteger(L, c->r);
	} else if (strcmp(key, "g") == 0) {
		lua_pushinteger(L, c->g);
	} else if (strcmp(key, "b") == 0) {
		lua_pushinteger(L, c->b);
	} else if (strcmp(key, "a") == 0) {
		lua_pushinteger(L, c->a);
	} else if (strcmp(key, "mix") == 0) {
		lua_pushcfunction(L, l_color_mix);
	} else if (strcmp(key, "darken") == 0) {
		lua_pushcfunction(L, l_color_darken);
	} else if (strcmp(key, "lighten") == 0) {
		lua_pushcfunction(L, l_color_lighten);
	} else {
		luaL_error(L, "unknown field '%s' on color", key);
		lua_pushnil(L);
	}
	return 1;
}

static int l_color_newindex(lua_State* L) {
	d_color* c = luaL_checkudata(L, 1, "color");
	const char* key = luaL_checkstring(L, 2);
	float value = luaL_checknumber(L, 3);
	if (strcmp(key, "r") == 0) {
		c->r = value;
	} else if (strcmp(key, "g") == 0) {
		c->g = value;
	} else if (strcmp(key, "b") == 0) {
		c->b = value;
	} else if (strcmp(key, "a") == 0) {
		c->a = value;
	}
	return 0;
}

static int l_color_tostring(lua_State* L) {
	d_color* c = luaL_checkudata(L, 1, "color");
	char buf[64];
	snprintf(buf, sizeof(buf), "color(%d, %d, %d, %d)", c->r, c->g, c->b, c->a);
	lua_pushstring(L, buf);
	return 1;
}

static int l_color_eq(lua_State* L) {
	d_color* c1 = luaL_checkudata(L, 1, "color");
	d_color* c2 = luaL_checkudata(L, 2, "color");
	lua_pushboolean(L, d_color_eq(*c1, *c2));
	return 1;
}

static luaL_Reg color_meta[] = {
	{ "__index", l_color_index },
	{ "__newindex", l_color_newindex },
	{ "__tostring", l_color_tostring },
	{ "__add", l_color_lighten },
	{ "__sub", l_color_darken },
	{ "__mul", l_color_mix },
	{ "__eq", l_color_eq },
	{ NULL, NULL, },
};

static int l_rng_rand(lua_State* L) {
	d_rng* rng = luaL_checkudata(L, 1, "rng");
	float a = luaL_optnumber(L, 1, 1);
	float b = luaL_optnumber(L, 2, 0);
	lua_pushnumber(L, d_rng_rand(rng, a, b));
	return 1;
}

static int l_rng_randi(lua_State* L) {
	d_rng* rng = luaL_checkudata(L, 1, "rng");
	int a = luaL_optnumber(L, 1, 2);
	int b = luaL_optnumber(L, 2, 0);
	lua_pushinteger(L, d_rng_randi(rng, a, b));
	return 1;
}

static int l_rng_index(lua_State* L) {
	d_rng* rng = luaL_checkudata(L, 1, "rng");
	const char* key = luaL_checkstring(L, 2);
	if (strcmp(key, "seed") == 0) {
		lua_pushinteger(L, rng->seed);
	} else if (strcmp(key, "rand") == 0) {
		lua_pushcfunction(L, l_rng_rand);
	} else if (strcmp(key, "randi") == 0) {
		lua_pushcfunction(L, l_rng_randi);
	} else {
		luaL_error(L, "unknown field '%s' on rng", key);
		lua_pushnil(L);
	}
	return 1;
}

static luaL_Reg rng_meta[] = {
	{ "__index", l_rng_index },
	{ NULL, NULL, },
};

static int l_vec2(lua_State* L) {
	float x = luaL_optnumber(L, 1, 0);
	float y = luaL_optnumber(L, 2, x);
	d_vec2 p = { x, y };
	lua_pushudata(L, d_vec2, "vec2", &p);
	return 1;
}

static int l_vec3(lua_State* L) {
	float x = luaL_optnumber(L, 1, 0);
	float y = luaL_optnumber(L, 2, x);
	float z = luaL_optnumber(L, 3, x);
	d_vec3 p = { x, y, z };
	lua_pushudata(L, d_vec3, "vec3", &p);
	return 1;
}

static int l_color(lua_State* L) {
	int nargs = lua_gettop(L);
	if (nargs == 1) {
		int x = luaL_checknumber(L, 1);
		d_color c = d_colorx(x);
		lua_pushudata(L, d_color, "color", &c);
	} else if (nargs == 3 || nargs == 4) {
		int r = luaL_checknumber(L, 1);
		int g = luaL_checknumber(L, 2);
		int b = luaL_checknumber(L, 3);
		int a = luaL_optnumber(L, 4, 255);
		d_color c = d_colori(r, g, b, a);
		lua_pushudata(L, d_color, "color", &c);
	} else {
		luaL_error(L, "wrong number of arguments to color: %d", nargs);
		lua_pushnil(L);
	}
	return 1;
}

static int l_rand(lua_State* L) {
	float a = luaL_optnumber(L, 1, 1);
	float b = luaL_optnumber(L, 2, 0);
	lua_pushnumber(L, d_randf(a, b));
	return 1;
}

static int l_randi(lua_State* L) {
	int a = luaL_optnumber(L, 1, 2);
	int b = luaL_optnumber(L, 2, 0);
	lua_pushinteger(L, d_randi(a, b));
	return 1;
}

static int l_chance(lua_State* L) {
	float c = luaL_optnumber(L, 1, 0.5);
	lua_pushboolean(L, d_chance(c));
	return 1;
}

static luaL_Reg global_funcs[] = {
	{ "vec2", l_vec2 },
	{ "vec3", l_vec3 },
	{ "color", l_color },
	{ "rand", l_rand },
	{ "randi", l_randi },
	{ "chance", l_chance },
	{ NULL, NULL, },
};

int main(int argc, char** argv) {

	char* path = NULL;
	int arg_start = -1;

	for (int i = 1; i < argc; i++) {
		char* arg = argv[i];
		if (strcmp(arg, "--") == 0) {
			arg_start = i + 1;
			break;
		} else if (arg[0] == '-') {
			// TODO
		} else {
			if (!path) {
				path = arg;
			}
		}
	}

	if (!path) {
		return EXIT_FAILURE;
	}

	lua_State* L = luaL_newstate();

	g.lua = L;
	luaL_openlibs(L);

	luaL_regtype(L, "img", img_meta);
	luaL_regtype(L, "model", model_meta);
	luaL_regtype(L, "sound", sound_meta);
	luaL_regtype(L, "playback", playback_meta);
	luaL_regtype(L, "vec2", vec2_meta);
	luaL_regtype(L, "vec3", vec3_meta);
	luaL_regtype(L, "color", color_meta);
	luaL_regtype(L, "rng", rng_meta);

	// d
	lua_newtable(L);

	luaL_newlib(L, app_funcs);
	lua_setfield(L, -2, "app");

	luaL_newlib(L, gfx_funcs);
	lua_setfield(L, -2, "gfx");

	luaL_newlib(L, audio_funcs);
	lua_setfield(L, -2, "audio");

	luaL_newlib(L, fs_funcs);
	lua_setfield(L, -2, "fs");

	luaL_newlib(L, col_funcs);
	lua_setfield(L, -2, "col");

	luaL_newlib(L, tween_funcs);
	lua_setfield(L, -2, "tween");

	luaL_newlib(L, ease_funcs);
	lua_setfield(L, -2, "ease");

	lua_setglobal(L, "d");

	luaL_regfuncs(L, global_funcs);

	// arg
	lua_newtable(L);

	if (arg_start != -1) {
		for (int i = arg_start; i < argc; i++) {
			lua_pushstring(L, argv[i]);
			lua_rawseti(L, -2, i - arg_start + 1);
		}
	}

	lua_setglobal(L, "arg");

	if (luaL_loadfile(L, path) || lua_pcall(L, 0, 0, 0)) {
		fprintf(stderr, "%s\n", lua_tostring(L, -1));
	}

	lua_close(L);

	return EXIT_SUCCESS;

}
