// wengwengweng

#ifndef D_GFX_H
#define D_GFX_H

#ifndef D_MATH_H
#error 'd_gfx.h' requires 'd_math.h'
#endif

typedef struct d_gfx_desc {
	int width;
	int height;
	bool no_depth_test;
	bool no_backface_cull;
	bool anti_alias;
	color clear_color;
} d_gfx_desc;

typedef enum {
	D_ORIG_CENTER,
	D_ORIG_TOP_LEFT,
	D_ORIG_TOP,
	D_ORIG_TOP_RIGHT,
	D_ORIG_LEFT,
	D_ORIG_RIGHT,
	D_ORIG_BOT_LEFT,
	D_ORIG_BOT,
	D_ORIG_BOT_RIGHT,
} d_origin;

typedef enum {
	D_BLEND_ALPHA,
	D_BLEND_REPLACE,
	D_BLEND_ADD,
} d_blend;

typedef enum {
	D_WRAP_BORDER,
	D_WRAP_EDGE,
	D_WRAP_REPEAT,
} d_wrap;

typedef struct {
	int width;
	int height;
	color* pixels;
} d_img;

typedef struct {
	int width;
	int height;
	int* buf;
} d_ibuf;

typedef struct {
	uint8_t* buf;
	int size;
} d_bitset;

// TODO: bitset might be slower in runtime
typedef struct {
	int width;
	int height;
	d_bitset bitset;
} d_bbuf;

d_bitset d_bitset_new(size_t size);
void d_bitset_set(d_bitset* m, int n, bool b);
bool d_bitset_get(d_bitset* m, int n);
void d_bitset_clear(d_bitset* m, bool b);
void d_bitset_free(d_bitset* m);

typedef struct {
	int gw;
	int gh;
	int charmap[128];
	uint8_t* bitmap;
} d_font;

typedef struct {
	vec3 pos;
	vec2 uv;
	vec3 normal;
	color color;
} d_vertex;

typedef uint32_t d_index;

typedef struct {
	d_vertex* verts;
	int num_verts;
	d_index* indices;
	int num_indices;
} d_mesh;

typedef struct {
	vec3 pos;
	quat rot;
	vec3 scale;
} d_transform;

typedef struct {
	mat4 transform;
	float time;
} d_model_anim_frame;

typedef struct {
	char* name;
	d_model_anim_frame* frames;
	int num_frames;
} d_model_anim;

typedef struct d_model_node {
	mat4 t;
	struct d_model_node* children;
	int num_children;
	d_mesh* meshes;
	int num_meshes;
	d_model_anim* anims;
	int num_anims;
} d_model_node;

typedef struct {
	d_model_node* nodes;
	int num_nodes;
	d_img* images;
	int num_images;
	box bbox;
	vec3 center;
} d_model;

typedef color(*d_gfx_shader)(color);

void d_gfx_init(d_gfx_desc);
#ifdef D_APP_H
void d_gfx_present();
vec2 d_gfx_mouse_pos();
vec2 d_gfx_mouse_dpos();
#endif

int d_gfx_width();
int d_gfx_height();

d_img d_img_new(int w, int h);
d_img d_img_parse(uint8_t* bytes, size_t size);
#ifdef D_FS_H
d_img d_img_load(char* path);
#endif
void d_img_set(d_img* img, int x, int y, color c);
color d_img_get(d_img* img, int x, int y);
void d_img_fill(d_img* img, color c);
void d_img_save(d_img* img, char* path);
d_img d_img_clone(d_img* img);
void d_img_free(d_img* img);

d_ibuf d_ibuf_new(int w, int h);
void d_ibuf_free(d_ibuf*);
void d_ibuf_set(d_ibuf* ibuf, int x, int y, int v);
int d_ibuf_get(d_ibuf* ibuf, int x, int y);
void d_ibuf_clear(d_ibuf* ibuf, int v);

d_bbuf d_bbuf_new(int w, int h);
void d_bbuf_free(d_bbuf*);
void d_bbuf_set(d_bbuf* bbuf, int x, int y, bool v);
bool d_bbuf_get(d_bbuf* bbuf, int x, int y);

d_mesh d_mesh_new(
	d_vertex* verts,
	int num_verts,
	d_index* indices,
	int num_indices
);
void d_mesh_free(d_mesh* mesh);
void d_mesh_gen_normals(d_mesh* mesh);

d_font d_font_parse(uint8_t* bytes);
#ifdef D_FS_H
d_font d_font_load(char* path);
#endif
void d_font_free(d_font* font);

d_model d_model_parse(uint8_t* bytes, int size);
#ifdef D_FS_H
d_model d_model_load(char* path);
#endif
void d_model_gen_bbox(d_model* model);
void d_model_free(d_model* model);

void d_gfx_clear();
void d_gfx_set_blend(d_blend b);
void d_gfx_set_wrap(d_wrap w);
void d_gfx_draw_pixel(int x, int y, int z, color c);
void d_gfx_blit_pixel(int x, int y, color c);
color d_gfx_get(int x, int y);
void d_blit_img(d_img* img, vec2 pos);
void d_blit_text(char* text, vec2 pos, color c);
void d_blit_bg();
void d_blit_rect(vec2 p1, vec2 p2, color c);
void d_blit_circle(vec2 center, float r, color c);
void d_blit_line(vec2 p1, vec2 p2, color c);
void d_draw_prim_tri(d_vertex v1, d_vertex v2, d_vertex v3, d_img* tex);
void d_draw_prim_quad(d_vertex v1, d_vertex v2, d_vertex v3, d_vertex v4, d_img* tex);
void d_draw_img(d_img* img, vec2 pos);
void d_draw_tri(vec2 p1, vec2 p2, vec2 p3, color c);
void d_draw_rect(vec2 p1, vec2 p2, color c);
void d_draw_line(vec2 p1, vec2 p2, color c);
void d_draw_line3(vec3 p1, vec3 p2, color c);
void d_draw_mesh(d_mesh* mesh, d_img* tex);
void d_draw_model(d_model* model);
void d_draw_bbox(box bbox, color c);
void d_gfx_t_push();
void d_gfx_t_pop();
void d_gfx_t_use(mat4 m);
void d_gfx_t_move(vec2 p);
void d_gfx_t_move3(vec3 p);
void d_gfx_t_scale(vec2 s);
void d_gfx_t_scale3(vec3 s);
void d_gfx_t_rot_x(float a);
void d_gfx_t_rot_y(float a);
void d_gfx_t_rot_z(float a);
vec2 d_gfx_t_apply_vec2(vec2 p);
vec3 d_gfx_t_apply_vec3(vec3 p);
void d_gfx_drawon(d_img* img);
d_img* d_gfx_canvas();
void d_gfx_set_shader(d_gfx_shader func);
void d_gfx_set_backface_cull(bool b);
void d_gfx_set_depth_test(bool b);
void d_gfx_set_bbuf_write(bool b);
void d_gfx_set_bbuf_test(bool b);
bool d_gfx_bbuf_get(int x, int y);
void d_gfx_bbuf_clear();

#endif // #ifndef D_GFX_H

#ifdef D_IMPL
#define D_GFX_IMPL
#endif

#ifdef D_GFX_IMPL
#ifndef D_GFX_IMPL_ONCE
#define D_GFX_IMPL_ONCE

#include <stdio.h>
#include <string.h>
#include <limits.h>

#define D_MAX_TSTACK 64

static uint8_t unscii_bytes[] = {
	0x64, 0x31, 0x52, 0x74, 0x79, 0x66, 0x30, 0x4e, 0x74, 0x08, 0x08, 0x5f,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b,
	0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
	0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43,
	0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b,
	0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
	0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73,
	0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18,
	0x00, 0x18, 0x00, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6c,
	0x6c, 0xfe, 0x6c, 0xfe, 0x6c, 0x6c, 0x00, 0x18, 0x3e, 0x60, 0x3c, 0x06,
	0x7c, 0x18, 0x00, 0x00, 0xc6, 0xcc, 0x18, 0x30, 0x66, 0xc6, 0x00, 0x38,
	0x6c, 0x38, 0x76, 0xdc, 0xcc, 0x76, 0x00, 0x18, 0x18, 0x30, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x0c, 0x18, 0x30, 0x30, 0x30, 0x18, 0x0c, 0x00, 0x30,
	0x18, 0x0c, 0x0c, 0x0c, 0x18, 0x30, 0x00, 0x00, 0x66, 0x3c, 0xff, 0x3c,
	0x66, 0x00, 0x00, 0x00, 0x18, 0x18, 0x7e, 0x18, 0x18, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x30, 0x00, 0x00, 0x00, 0x7e, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x03,
	0x06, 0x0c, 0x18, 0x30, 0x60, 0xc0, 0x00, 0x3c, 0x66, 0x6e, 0x76, 0x66,
	0x66, 0x3c, 0x00, 0x18, 0x38, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x00, 0x3c,
	0x66, 0x0c, 0x18, 0x30, 0x60, 0x7e, 0x00, 0x3c, 0x66, 0x06, 0x1c, 0x06,
	0x66, 0x3c, 0x00, 0x1c, 0x3c, 0x6c, 0xcc, 0xfe, 0x0c, 0x0c, 0x00, 0x7e,
	0x60, 0x7c, 0x06, 0x06, 0x66, 0x3c, 0x00, 0x1c, 0x30, 0x60, 0x7c, 0x66,
	0x66, 0x3c, 0x00, 0x7e, 0x06, 0x06, 0x0c, 0x18, 0x18, 0x18, 0x00, 0x3c,
	0x66, 0x66, 0x3c, 0x66, 0x66, 0x3c, 0x00, 0x3c, 0x66, 0x66, 0x3e, 0x06,
	0x0c, 0x38, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00,
	0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x30, 0x0c, 0x18, 0x30, 0x60, 0x30,
	0x18, 0x0c, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x60,
	0x30, 0x18, 0x0c, 0x18, 0x30, 0x60, 0x00, 0x3c, 0x66, 0x06, 0x0c, 0x18,
	0x00, 0x18, 0x00, 0x7c, 0xc6, 0xde, 0xde, 0xde, 0xc0, 0x7c, 0x00, 0x18,
	0x3c, 0x66, 0x66, 0x7e, 0x66, 0x66, 0x00, 0x7c, 0x66, 0x66, 0x7c, 0x66,
	0x66, 0x7c, 0x00, 0x3c, 0x66, 0x60, 0x60, 0x60, 0x66, 0x3c, 0x00, 0x78,
	0x6c, 0x66, 0x66, 0x66, 0x6c, 0x78, 0x00, 0x7e, 0x60, 0x60, 0x7c, 0x60,
	0x60, 0x7e, 0x00, 0x7e, 0x60, 0x60, 0x7c, 0x60, 0x60, 0x60, 0x00, 0x3c,
	0x66, 0x60, 0x6e, 0x66, 0x66, 0x3e, 0x00, 0x66, 0x66, 0x66, 0x7e, 0x66,
	0x66, 0x66, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x00, 0x06,
	0x06, 0x06, 0x06, 0x06, 0x66, 0x3c, 0x00, 0xc6, 0xcc, 0xd8, 0xf0, 0xd8,
	0xcc, 0xc6, 0x00, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x7e, 0x00, 0xc6,
	0xee, 0xfe, 0xd6, 0xc6, 0xc6, 0xc6, 0x00, 0xc6, 0xe6, 0xf6, 0xde, 0xce,
	0xc6, 0xc6, 0x00, 0x3c, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00, 0x7c,
	0x66, 0x66, 0x7c, 0x60, 0x60, 0x60, 0x00, 0x3c, 0x66, 0x66, 0x66, 0x66,
	0x6c, 0x36, 0x00, 0x7c, 0x66, 0x66, 0x7c, 0x6c, 0x66, 0x66, 0x00, 0x3c,
	0x66, 0x60, 0x3c, 0x06, 0x66, 0x3c, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18,
	0x18, 0x18, 0x00, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00, 0x66,
	0x66, 0x66, 0x66, 0x66, 0x3c, 0x18, 0x00, 0xc6, 0xc6, 0xc6, 0xd6, 0xfe,
	0xee, 0xc6, 0x00, 0xc3, 0x66, 0x3c, 0x18, 0x3c, 0x66, 0xc3, 0x00, 0xc3,
	0x66, 0x3c, 0x18, 0x18, 0x18, 0x18, 0x00, 0x7e, 0x06, 0x0c, 0x18, 0x30,
	0x60, 0x7e, 0x00, 0x3c, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3c, 0x00, 0xc0,
	0x60, 0x30, 0x18, 0x0c, 0x06, 0x03, 0x00, 0x3c, 0x0c, 0x0c, 0x0c, 0x0c,
	0x0c, 0x3c, 0x00, 0x10, 0x38, 0x6c, 0xc6, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x18, 0x0c, 0x06, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x06, 0x3e, 0x66, 0x3e, 0x00, 0x60,
	0x60, 0x7c, 0x66, 0x66, 0x66, 0x7c, 0x00, 0x00, 0x00, 0x3c, 0x60, 0x60,
	0x60, 0x3c, 0x00, 0x06, 0x06, 0x3e, 0x66, 0x66, 0x66, 0x3e, 0x00, 0x00,
	0x00, 0x3c, 0x66, 0x7e, 0x60, 0x3c, 0x00, 0x1c, 0x30, 0x7c, 0x30, 0x30,
	0x30, 0x30, 0x00, 0x00, 0x00, 0x3e, 0x66, 0x66, 0x3e, 0x06, 0x7c, 0x60,
	0x60, 0x7c, 0x66, 0x66, 0x66, 0x66, 0x00, 0x18, 0x00, 0x38, 0x18, 0x18,
	0x18, 0x1e, 0x00, 0x0c, 0x00, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x78, 0x60,
	0x60, 0x66, 0x6c, 0x78, 0x6c, 0x66, 0x00, 0x38, 0x18, 0x18, 0x18, 0x18,
	0x18, 0x1e, 0x00, 0x00, 0x00, 0xcc, 0xfe, 0xd6, 0xd6, 0xc6, 0x00, 0x00,
	0x00, 0x7c, 0x66, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x3c, 0x66, 0x66,
	0x66, 0x3c, 0x00, 0x00, 0x00, 0x7c, 0x66, 0x66, 0x7c, 0x60, 0x60, 0x00,
	0x00, 0x3e, 0x66, 0x66, 0x3e, 0x06, 0x06, 0x00, 0x00, 0x7c, 0x66, 0x60,
	0x60, 0x60, 0x00, 0x00, 0x00, 0x3e, 0x60, 0x3c, 0x06, 0x7c, 0x00, 0x30,
	0x30, 0x7e, 0x30, 0x30, 0x30, 0x1e, 0x00, 0x00, 0x00, 0x66, 0x66, 0x66,
	0x66, 0x3e, 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x3c, 0x18, 0x00, 0x00,
	0x00, 0xc6, 0xc6, 0xd6, 0x7c, 0x6c, 0x00, 0x00, 0x00, 0xc6, 0x6c, 0x38,
	0x6c, 0xc6, 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x3e, 0x06, 0x3c, 0x00,
	0x00, 0x7e, 0x0c, 0x18, 0x30, 0x7e, 0x00, 0x0e, 0x18, 0x18, 0x70, 0x18,
	0x18, 0x0e, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x70,
	0x18, 0x18, 0x0e, 0x18, 0x18, 0x70, 0x00, 0x76, 0xdc, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00
};

typedef struct {
	d_img def_canvas;
	d_img* cur_canvas;
	d_ibuf depth_buf;
	d_bbuf bbuf;
	bool depth_test;
	bool backface_cull;
	bool anti_alias;
	bool bbuf_write;
	bool bbuf_test;
	d_font def_font;
	d_font* cur_font;
	color clear_color;
	d_blend blend;
	d_wrap wrap;
	mat4 t;
	mat4 tstack[D_MAX_TSTACK];
	int tstack_len;
} d_gfx_ctx;

static d_gfx_ctx d_gfx;

void d_gfx_init(d_gfx_desc desc) {
	d_gfx.def_canvas = d_img_new(desc.width, desc.height);
	d_gfx.cur_canvas = &d_gfx.def_canvas;
	d_gfx.depth_buf = d_ibuf_new(desc.width, desc.height);
	d_gfx.bbuf = d_bbuf_new(desc.width, desc.height);
	d_gfx.depth_test = !desc.no_depth_test;
	d_gfx.backface_cull = !desc.no_backface_cull;
	d_gfx.anti_alias = desc.anti_alias;
	d_gfx.bbuf_write = false;
	d_gfx.bbuf_test = false;
	d_gfx.def_font = d_font_parse(unscii_bytes);
	d_gfx.cur_font = &d_gfx.def_font;
	d_gfx.blend = D_BLEND_ALPHA;
	d_gfx.wrap = D_WRAP_BORDER;
	d_gfx.clear_color = desc.clear_color;
	d_gfx.t = mat4u();
	memset(d_gfx.tstack, 0, sizeof(mat4) * D_MAX_TSTACK);
	d_gfx.tstack_len = 0;
	d_gfx_clear();
}

#ifdef D_APP_H

vec2 d_gfx_mouse_pos() {
	vec2 mpos = d_app_mouse_pos();
	return vec2f(
		mpos.x * d_gfx_width() / d_app_width(),
		mpos.y * d_gfx_height() / d_app_height()
	);
}

vec2 d_gfx_mouse_dpos() {
	vec2 mdpos = d_app_mouse_dpos();
	return vec2f(
		mdpos.x * d_gfx_width() / d_app_width(),
		mdpos.y * d_gfx_height() / d_app_height()
	);
}

void d_gfx_present() {
	d_img* c = d_gfx.cur_canvas;
	d_app_present(c->width, c->height, c->pixels);
	d_gfx.cur_canvas = &d_gfx.def_canvas;
}

#endif // #ifdef D_APP_H

int d_gfx_width() {
	return d_gfx.cur_canvas->width;
}

int d_gfx_height() {
	return d_gfx.cur_canvas->height;
}

d_ibuf d_ibuf_new(int w, int h) {
	return (d_ibuf) {
		.width = w,
		.height = h,
		.buf = calloc(w * h, sizeof(int)),
	};
}

void d_ibuf_free(d_ibuf* ibuf) {
	free(ibuf->buf);
	memset(ibuf, 0, sizeof(d_ibuf));
}

void d_ibuf_set(d_ibuf* ibuf, int x, int y, int v) {
	ibuf->buf[y * ibuf->width + x] = v;
}

int d_ibuf_get(d_ibuf* ibuf, int x, int y) {
	return ibuf->buf[y * ibuf->width + x];
}

void d_ibuf_clear(d_ibuf* ibuf, int v) {
	for (int i = 0; i < ibuf->width * ibuf->height; i++) {
		ibuf->buf[i] = v;
	}
}

d_bbuf d_bbuf_new(int w, int h) {
	return (d_bbuf) {
		.width = w,
		.height = h,
		.bitset = d_bitset_new(w * h),
	};
}

void d_bbuf_free(d_bbuf* bbuf) {
	d_bitset_free(&bbuf->bitset);
	memset(bbuf, 0, sizeof(d_bbuf));
}

void d_bbuf_set(d_bbuf* bbuf, int x, int y, bool b) {
	d_bitset_set(&bbuf->bitset, y * bbuf->width + x, b);
}

bool d_bbuf_get(d_bbuf* bbuf, int x, int y) {
	return d_bitset_get(&bbuf->bitset, y * bbuf->width + x);
}

void d_bbuf_clear(d_bbuf* bbuf, bool b) {
	d_bitset_clear(&bbuf->bitset, b);
}

d_bitset d_bitset_new(size_t size) {
	size = (size_t)ceil(size / 8.0f);
	uint8_t* buf = malloc(size);
	memset(buf, 0, size);
	return (d_bitset) {
		.buf = buf,
		.size = size,
	};
}

// TODO: is << faster than an uint8_t[8] lookup?
void d_bitset_set(d_bitset* m, int n, bool b) {
	if (b) {
		m->buf[n / 8] |= (1 << (7 - n % 8));
	} else {
		m->buf[n / 8] ^= (1 << (7 - n % 8));
	}
}

bool d_bitset_get(d_bitset* m, int n) {
	return m->buf[n / 8] & (1 << (7 - n % 8));
}

void d_bitset_clear(d_bitset* m, bool b) {
	uint8_t n = b ? 0xff : 0x00;
	for (int i = 0; i < m->size; i++) {
		m->buf[i] = n;
	}
}

void d_bitset_free(d_bitset* m) {
	free(m->buf);
}

d_img d_img_new(int w, int h) {
	return (d_img) {
		.width = w,
		.height = h,
		.pixels = calloc(w * h, sizeof(color)),
	};
}

d_img d_img_empty() {
	return (d_img) {
		.width = 0,
		.height = 0,
		.pixels = malloc(0),
	};
}

static uint8_t png_sig[] = { 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a };
static uint8_t jpeg_sig[] = { 0xff, 0xd8, 0xff };

d_img d_img_parse(uint8_t* bytes, size_t size) {
	if (
		memcmp(bytes, png_sig, sizeof(png_sig)) == 0
		|| memcmp(bytes, jpeg_sig, sizeof(jpeg_sig)) == 0
	) {
#ifdef STB_IMAGE_IMPLEMENTATION
		int w;
		int h;
		uint8_t* pixels = stbi_load_from_memory(bytes, size, &w, &h, NULL, 4);
		return (d_img) {
			.width = w,
			.height = h,
			.pixels = (color*)pixels,
		};
#else
		fprintf(stderr, "jpeg / png support requires 'stb_image.h'\n");
		return d_img_empty();
#endif // #ifdef STB_IMAGE_IMPLEMENTATION
	} else {
		fprintf(stderr, "unsupported image format\n");
		return d_img_empty();
	}
}

#ifdef D_FS_H
d_img d_img_load(char* path) {
	size_t size;
	uint8_t* bytes = d_read_bytes(path, &size);
	if (!bytes) {
		fprintf(stderr, "failed to load img from '%s'\n", path);
		return d_img_empty();
	}
	d_img img = d_img_parse(bytes, size);
	free(bytes);
	return img;
}
#endif // #ifdef D_FS_H

void d_img_set(d_img* img, int x, int y, color c) {
	img->pixels[y * img->width + x] = c;
}

color d_img_get(d_img* img, int x, int y) {
	return img->pixels[y * img->width + x];
}

void d_img_fill(d_img* img, color c) {
	for (int i = 0; i < img->width * img->height; i++) {
		img->pixels[i] = c;
	}
}

void d_img_save(d_img* img, char* path) {

#ifdef STB_IMAGE_WRITE_IMPLEMENTATION
	stbi_write_png(
		path,
		img->width,
		img->height,
		4,
		(uint8_t*)img->pixels,
		img->width * sizeof(color)
	);
#else
	fprintf(stderr, "image save only available with 'stb_image_write.h'\n");
#endif // #ifdef STB_IMAGE_WRITE_IMPLEMENTATION

}

d_img d_img_clone(d_img* img) {
	int w = img->width;
	int h = img->height;
	d_img img2 = d_img_new(w, h);
	memcpy(img2.pixels, img->pixels, w * h * sizeof(color));
	return img2;
}

void d_img_free(d_img* img) {
	free(img->pixels);
	memset(img, 0, sizeof(d_img));
}

d_font d_font_parse(uint8_t* bytes) {

	d_font f;
	memset(&f, 0, sizeof(d_font));

	f.gw = bytes[9];
	f.gh = bytes[10];
	uint8_t chars_size = bytes[11];
	size_t bitmap_size = ceil(chars_size * f.gw * f.gh / 8.0f);
	uint8_t* chars_buf = bytes + 9 + 1 + 1 + 1;
	uint8_t* bitmap_buf = chars_buf + chars_size;
	f.bitmap = calloc(chars_size * f.gw * f.gh, 1);

	for (int i = 0; i < bitmap_size; i++) {
		for (int j = 0; j < 8; j++) {
			if (bitmap_buf[i] & (1 << (7 - j))) {
				f.bitmap[(i * 8 + j)] = 1;
			}
		}
	}

	for (int i = 0; i < chars_size; i++) {
		f.charmap[(int)chars_buf[i]] = i;
	}

	return f;

}

d_font d_font_empty() {
	return (d_font) {
		.gw = 0,
		.gh = 0,
		.charmap = {0},
		.bitmap = malloc(0),
	};
}

#ifdef D_FS_H
d_font d_font_load(char* path) {
	size_t size;
	uint8_t* bytes = d_read_bytes(path, &size);
	if (!bytes) {
		fprintf(stderr, "failed to load font from '%s'\n", path);
		return (d_font) {0};
	}
	d_font font = d_font_parse(bytes);
	free(bytes);
	return font;
}
#endif // #ifdef D_FS_H

void d_font_free(d_font* f) {
	free(f->bitmap);
	memset(f, 0, sizeof(d_font));
}

void d_gfx_clear() {
	d_img_fill(d_gfx.cur_canvas, d_gfx.clear_color);
	d_ibuf_clear(&d_gfx.depth_buf, -INT_MAX);
}

void d_gfx_bbuf_clear() {
	d_bbuf_clear(&d_gfx.bbuf, false);
}

void d_gfx_set_backface_cull(bool b) {
	d_gfx.backface_cull = b;
}

void d_gfx_set_depth_test(bool b) {
	d_gfx.depth_test = b;
}

void d_gfx_set_bbuf_write(bool b) {
	d_gfx.bbuf_write = b;
}

void d_gfx_set_bbuf_test(bool b) {
	d_gfx.bbuf_test = b;
}

bool d_gfx_bbuf_get(int x, int y) {
	return d_bbuf_get(&d_gfx.bbuf, x, y);
}

void d_gfx_draw_pixel(int x, int y, int z, color c) {
	d_img* img = d_gfx.cur_canvas;
	if (x < 0 || x >= img->width || y < 0 || y >= img->height) {
		return;
	}
	if (d_gfx.depth_test) {
		if (d_ibuf_get(&d_gfx.depth_buf, x, y) <= z) {
			d_ibuf_set(&d_gfx.depth_buf, x, y, z);
		} else {
			return;
		}
	}
	if (d_gfx.bbuf_write) {
		if (c.a != 0) {
			d_bbuf_set(&d_gfx.bbuf, x, y, true);
		}
	}
	if (d_gfx.bbuf_test) {
		if (!d_bbuf_get(&d_gfx.bbuf, x, y)) {
			return;
		}
	}
	int i = y * img->width + x;
	switch (d_gfx.blend) {
		case D_BLEND_ALPHA: {
			if (c.a == 255) {
				img->pixels[i] = c;
			} else if (c.a != 0) {
				color rc = img->pixels[i];
				img->pixels[i] = (color) {
					.r = (rc.r * (255 - c.a) + c.r * c.a) / 255,
					.g = (rc.g * (255 - c.a) + c.g * c.a) / 255,
					.b = (rc.b * (255 - c.a) + c.b * c.a) / 255,
					.a = (rc.a * (255 - c.a) + c.a * c.a) / 255,
				};
			}
			break;
		}
		case D_BLEND_REPLACE:
			img->pixels[i] = c;
			break;
		case D_BLEND_ADD:
			if (c.a != 0) {
				color rc = img->pixels[i];
				img->pixels[i] = (color) {
					.r = (rc.r * rc.a + c.r * c.a) / 255,
					.g = (rc.g * rc.a + c.g * c.a) / 255,
					.b = (rc.b * rc.a + c.b * c.a) / 255,
					.a = (rc.a * rc.a + c.a * c.a) / 255,
				};
			}
			break;
	}
}

void d_gfx_blit_pixel(int x, int y, color c) {
	d_img* img = d_gfx.cur_canvas;
	if (x < 0 || x >= img->width || y < 0 || y >= img->height) {
		return;
	}
	if (c.a != 0) {
		img->pixels[y * img->width + x] = c;
	}
}

color d_gfx_get(int x, int y) {
	d_img* img = d_gfx.cur_canvas;
	switch (d_gfx.wrap) {
		case D_WRAP_BORDER:
			if (x < 0 || x >= img->width || y < 0 || y >= img->height) {
				return colori(0, 0, 0, 0);
			} else {
				return img->pixels[y * img->width + x];
			}
		case D_WRAP_EDGE: {
			int xx = clampi(x, 0, img->width - 1);
			int yy = clampi(y, 0, img->height - 1);
			return img->pixels[yy * img->width + xx];
		}
		case D_WRAP_REPEAT: {
			int xx = x % img->width;
			int yy = y % img->height;
			return img->pixels[yy * img->width + xx];
		}
	}
}

void d_blit_img(d_img* img, vec2 pos) {
	for (int x = 0; x < img->width; x++) {
		for (int y = 0; y < img->height; y++) {
			d_gfx_blit_pixel(
				x + pos.x,
				y + pos.y,
				img->pixels[y * img->width + x]
			);
		}
	}
}

void d_blit_bg() {
	color c1 = colori(128, 128, 128, 255);
	color c2 = colori(191, 191, 191, 255);
	int s = 32;
	for (int x = 0; x < d_gfx_width(); x++) {
		for (int y = 0; y < d_gfx_height(); y++) {
			color c = (x / s % 2 + y / s % 2) == 1 ? c1 : c2;
			d_gfx_blit_pixel(x, y, c);
		}
	}
}

void d_blit_rect(vec2 p1, vec2 p2, color c) {
	int x1 = p1.x < p2.x ? p1.x : p2.x;
	int x2 = p1.x > p2.x ? p1.x : p2.x;
	int y1 = p1.y < p2.y ? p1.y : p2.y;
	int y2 = p1.y > p2.y ? p1.y : p2.y;
	for (int x = x1; x < x2; x++) {
		for (int y = y1; y < y2; y++) {
			d_gfx_blit_pixel(x, y, c);
		}
	}
}

void d_blit_circle(vec2 center, float r, color c) {
	for (int i = center.x - r; i <= center.x + r; i++) {
		for (int j = center.y - r; j <= center.y + r; j++) {
			vec2 p = vec2f(i, j);
			float d = vec2_dist(p, center);
			if (d <= r) {
				d_gfx_blit_pixel(p.x, p.y, c);
			}
		}
	}
}

void d_blit_text(char* text, vec2 pos, color c) {

	int num_chars = strlen(text);
	d_font* font = &d_gfx.def_font;
	int ox = 0;

	for (int i = 0; i < num_chars; i++) {

		int charpos = font->charmap[(int)text[i]];

		for (int x = 0; x < font->gw; x++) {
			for (int y = 0; y < font->gh; y++) {
				if (font->bitmap[(charpos * font->gh + y) * font->gw + x] == 1) {
					d_gfx_blit_pixel(pos.x + ox + x, pos.y + y, c);
				}
			}
		}

		ox += font->gw;

	}

}

void d_blit_line(vec2 p1, vec2 p2, color c) {

	int x1 = p1.x;
	int y1 = p1.y;
	int x2 = p2.x;
	int y2 = p2.y;
	bool steep = abs(y2 - y1) > abs(x2 - x1);

	if (steep) {
		swapi(&x1, &y1);
		swapi(&x2, &y2);
	}

	if (x1 > x2) {
		swapi(&x1, &x2);
		swapi(&y1, &y2);
	}

	int dx = x2 - x1;
	int dy = abs(y2 - y1);
	int err = dx / 2;
	int step = y1 < y2 ? 1 : -1;
	int y = y1;

	for (int x = x1; x <= x2; x++) {
		if (steep) {
			d_gfx_blit_pixel(y, x, c);
		} else {
			d_gfx_blit_pixel(x, y, c);
		}
		err -= dy;
		if (err < 0) {
			y += step;
			err += dx;
		}
	}

}

void d_vertex_swap(d_vertex* v1, d_vertex* v2) {
	d_vertex v3 = *v2;
	*v2 = *v1;
	*v1 = v3;
}

void d_draw_prim_tri(
	d_vertex v1,
	d_vertex v2,
	d_vertex v3,
	d_img* tex
) {

	vec3 p1 = d_gfx_t_apply_vec3(v1.pos);
	vec3 p2 = d_gfx_t_apply_vec3(v2.pos);
	vec3 p3 = d_gfx_t_apply_vec3(v3.pos);

	if (d_gfx.backface_cull) {
		vec3 normal = vec3_unit(
			vec3_cross(
				vec3_sub(p3, p1),
				vec3_sub(p2, p1)
			)
		);
		if (vec3_dot(normal, vec3f(0, 0, 1)) < 0) {
			return;
		}
	}

	if (p1.y > p2.y) {
		d_vertex_swap(&v1, &v2);
		vec3_swap(&p1, &p2);
	}

	if (p1.y > p3.y) {
		d_vertex_swap(&v1, &v3);
		vec3_swap(&p1, &p3);
	}

	if (p2.y > p3.y) {
		d_vertex_swap(&v2, &v3);
		vec3_swap(&p2, &p3);
	}

	int x1 = p1.x;
	int y1 = p1.y;
	int z1 = p1.z;
	int x2 = p2.x;
	int y2 = p2.y;
	int z2 = p2.z;
	int x3 = p3.x;
	int y3 = p3.y;
	int z3 = p3.z;

// 	vec3 n1 = vec3_unit(d_gfx_t_apply_vec3(v1.normal));
// 	vec3 n2 = vec3_unit(d_gfx_t_apply_vec3(v2.normal));
// 	vec3 n3 = vec3_unit(d_gfx_t_apply_vec3(v3.normal));

	int gw = d_gfx_width();
	int gh = d_gfx_height();

	for (int y = y1; y < y3; y++) {

		if (y < 0 || y >= gh) {
			continue;
		}

		bool prebend = y < y2;

		int x_start = prebend
			? mapi(y, y1, y2, x1, x2)
			: mapi(y, y2, y3, x2, x3);
		int x_end = mapi(y, y1, y3, x1, x3);

		int z_start = prebend
			? mapi(y, y1, y2, z1, z2)
			: mapi(y, y2, y3, z2, z3);
		int z_end = mapi(y, y1, y3, z1, z3);

		float ty1 = (float)(y - y1) / (float)(y2 - y1);
		float ty2 = (float)(y - y2) / (float)(y3 - y2);
		float ty3 = (float)(y - y1) / (float)(y3 - y1);

		color col_start = prebend
			? color_lerp(v1.color, v2.color, ty1)
			: color_lerp(v2.color, v3.color, ty2);
		color col_end = color_lerp(v1.color, v3.color, ty3);

		vec2 uv_start = prebend
			? vec2_lerp(v1.uv, v2.uv, ty1)
			: vec2_lerp(v2.uv, v3.uv, ty2);
		vec2 uv_end = vec2_lerp(v1.uv, v3.uv, ty3);

// 		vec3 normal_start = prebend
// 			? vec3_lerp(n1, n2, ty1)
// 			: vec3_lerp(n2, n3, ty2);
// 		vec3 normal_end = vec3_lerp(n1, n3, ty3);

		bool same_color = color_eq(col_start, col_end);
		int x_len = x_end - x_start;

		for (int x = mini(x_start, x_end); x < maxi(x_start, x_end); x++) {

			if (x < 0 || x >= gw) {
				continue;
			}

			int z = mapi(x, x_start, x_end, z_start, z_end);
			float t = (float)(x - x_start) / (float)x_len;

			color c = same_color
				? col_start
				: color_lerp(col_start, col_end, t);

// 			vec3 normal = vec3_lerp(normal_start, normal_end, t);
// 			int l = mapf(normal.x + normal.y + normal.z, 3, -3, -255, 255);
// 			c = color_lighten(c, l);

			if (tex) {
				vec2 uv = vec2_lerp(uv_start, uv_end, t);
				c = color_mix(
					d_img_get(tex, tex->width * uv.x, tex->height * uv.y),
					c
				);
			}

			d_gfx_draw_pixel(x, y, z, c);

		}

	}

}

void d_draw_prim_quad(
	d_vertex v1,
	d_vertex v2,
	d_vertex v3,
	d_vertex v4,
	d_img* tex
) {
	d_draw_prim_tri(v1, v2, v3, tex);
	d_draw_prim_tri(v1, v3, v4, tex);
}

void d_draw_img(d_img* img, vec2 pos) {
	d_draw_prim_quad(
		(d_vertex) {
			.pos = vec3f(pos.x, pos.y, 0),
			.color = colorx(0xffffffff),
			.uv = vec2f(0, 0),
			.normal = vec3f(0, 0, 1),
		},
		(d_vertex) {
			.pos = vec3f(pos.x + img->width, pos.y, 0),
			.color = colorx(0xffffffff),
			.uv = vec2f(1, 0),
			.normal = vec3f(0, 0, 1),
		},
		(d_vertex) {
			.pos = vec3f(pos.x + img->width, pos.y + img->height, 0),
			.color = colorx(0xffffffff),
			.uv = vec2f(1, 1),
			.normal = vec3f(0, 0, 1),
		},
		(d_vertex) {
			.pos = vec3f(pos.x, pos.y + img->height, 0),
			.color = colorx(0xffffffff),
			.uv = vec2f(0, 1),
			.normal = vec3f(0, 0, 1),
		},
		img
	);
}

void d_draw_tri(vec2 p1, vec2 p2, vec2 p3, color c) {
	d_draw_prim_tri(
		(d_vertex) {
			.pos = vec3f(p1.x, p1.y, 0),
			.color = c,
			.normal = vec3f(0, 0, 1),
		},
		(d_vertex) {
			.pos = vec3f(p2.x, p2.y, 0),
			.color = c,
			.normal = vec3f(0, 0, 1),
		},
		(d_vertex) {
			.pos = vec3f(p3.x, p3.y, 0),
			.color = c,
			.normal = vec3f(0, 0, 1),
		},
		NULL
	);
}

void d_draw_rect(vec2 p1, vec2 p2, color c) {
	d_draw_prim_quad(
		(d_vertex) {
			.pos = vec3f(p1.x, p1.y, 0),
			.color = c,
			.normal = vec3f(0, 0, 1),
		},
		(d_vertex) {
			.pos = vec3f(p2.x, p1.y, 0),
			.color = c,
			.normal = vec3f(0, 0, 1),
		},
		(d_vertex) {
			.pos = vec3f(p2.x, p2.y, 0),
			.color = c,
			.normal = vec3f(0, 0, 1),
		},
		(d_vertex) {
			.pos = vec3f(p1.x, p2.y, 0),
			.color = c,
			.normal = vec3f(0, 0, 1),
		},
		NULL
	);
}

void d_draw_line(vec2 p1, vec2 p2, color c) {
	p1 = d_gfx_t_apply_vec2(p1);
	p2 = d_gfx_t_apply_vec2(p2);
	d_blit_line(p1, p2, c);
}

void d_draw_line3(vec3 p1, vec3 p2, color c) {
	p1 = d_gfx_t_apply_vec3(p1);
	p2 = d_gfx_t_apply_vec3(p2);
	d_blit_line(vec2f(p1.x, p1.y), vec2f(p2.x, p2.y), c);
}

// TODO
void d_draw_line2(vec2 p1, vec2 p2, int w, color c) {
	vec2 d = vec2_scale(vec2_unit(vec2_normal(vec2_sub(p1, p2))), w);
	vec2 pp1 = vec2_add(p1, d);
	vec2 pp2 = vec2_sub(p1, d);
	vec2 pp3 = vec2_sub(p2, d);
	vec2 pp4 = vec2_add(p2, d);
	d_draw_prim_quad(
		(d_vertex) {
			.pos = vec3f(pp1.x, pp1.y, 0),
			.color = c,
			.normal = vec3f(0, 0, 1),
		},
		(d_vertex) {
			.pos = vec3f(pp2.x, pp2.y, 0),
			.color = c,
			.normal = vec3f(0, 0, 1),
		},
		(d_vertex) {
			.pos = vec3f(pp3.x, pp3.y, 0),
			.color = c,
			.normal = vec3f(0, 0, 1),
		},
		(d_vertex) {
			.pos = vec3f(pp4.x, pp4.y, 0),
			.color = c,
			.normal = vec3f(0, 0, 1),
		},
		NULL
	);
}

void d_gfx_t_push() {
	if (d_gfx.tstack_len >= D_MAX_TSTACK) {
		fprintf(stderr, "tstack overflow\n");
		return;
	}
	d_gfx.tstack[d_gfx.tstack_len++] = d_gfx.t;
}

void d_gfx_t_pop() {
	if (d_gfx.tstack_len <= 0) {
		fprintf(stderr, "tstack underflow\n");
		return;
	}
	d_gfx.t = d_gfx.tstack[--d_gfx.tstack_len];
}

void d_gfx_t_use(mat4 m) {
	d_gfx.t = mat4_mult(d_gfx.t, m);
}

void d_gfx_t_move(vec2 p) {
	d_gfx_t_move3(vec3f(p.x, p.y, 0));
}

void d_gfx_t_scale(vec2 s) {
	d_gfx_t_scale3(vec3f(s.x, s.y, 1));
}

void d_gfx_t_move3(vec3 p) {
	d_gfx.t = mat4_mult(d_gfx.t, mat4_translate(p));
}

void d_gfx_t_scale3(vec3 s) {
	d_gfx.t = mat4_mult(d_gfx.t, mat4_scale(s));
}

void d_gfx_t_rot_x(float a) {
	d_gfx.t = mat4_mult(d_gfx.t, mat4_rot_x(a));
}

void d_gfx_t_rot_y(float a) {
	d_gfx.t = mat4_mult(d_gfx.t, mat4_rot_y(a));
}

void d_gfx_t_rot_z(float a) {
	d_gfx.t = mat4_mult(d_gfx.t, mat4_rot_z(a));
}

vec2 d_gfx_t_apply_vec2(vec2 p) {
	return mat4_mult_vec2(d_gfx.t, p);
}

vec3 d_gfx_t_apply_vec3(vec3 p) {
	return mat4_mult_vec3(d_gfx.t, p);
}

void d_draw_mesh(d_mesh* mesh, d_img* tex) {
	for (int i = 0; i < mesh->num_indices; i += 3) {
		d_draw_prim_tri(
			mesh->verts[mesh->indices[i + 0]],
			mesh->verts[mesh->indices[i + 1]],
			mesh->verts[mesh->indices[i + 2]],
			tex
		);
	}
}

void d_draw_mesh_line(d_mesh* mesh, color c) {
	for (int i = 0; i < mesh->num_indices; i += 3) {
		d_vertex v1 = mesh->verts[mesh->indices[i + 0]];
		d_vertex v2 = mesh->verts[mesh->indices[i + 1]];
		d_vertex v3 = mesh->verts[mesh->indices[i + 2]];
		d_draw_line3(v1.pos, v2.pos, c);
		d_draw_line3(v2.pos, v3.pos, c);
		d_draw_line3(v3.pos, v1.pos, c);
	}
}

static mat4 d_transform_mat4(d_transform t) {
	return mat4_mult(
		mat4_mult(
			mat4_translate(t.pos),
			mat4_scale(t.scale)
		),
		mat4_rot_quat(t.rot)
	);
}

static d_transform d_transform_apply(d_transform t1, d_transform t2) {
	return (d_transform) {
		.pos = vec3_add(t1.pos, t2.pos),
		.rot = quat_mult(t1.rot, t2.rot),
		.scale = vec3_mult(t1.scale, t2.scale),
	};
}

static void d_draw_model_node(d_model* model, d_model_node* node) {
	d_gfx_t_push();
	// TODO
	if (node->num_anims) {
		d_model_anim* anim = &node->anims[0];
		d_gfx_t_use(anim->frames[(int)(d_app_time() * 60) % anim->num_frames].transform);
	} else {
		d_gfx_t_use(node->t);
	}
	for (int i = 0; i < node->num_meshes; i++) {
		d_draw_mesh(
			&node->meshes[i],
			model->num_images > 0 ? &model->images[0] : NULL
		);
	}
	for (int i = 0; i < node->num_children; i++) {
		d_draw_model_node(model, &node->children[i]);
	}
	d_gfx_t_pop();
}

void d_draw_model(d_model* model) {
	d_gfx_t_push();
	for (int i = 0; i < model->num_nodes; i++) {
		d_draw_model_node(model, &model->nodes[i]);
	}
	d_gfx_t_pop();
}

static void d_draw_model_node_line(d_model_node* node, color c) {
	d_gfx_t_push();
	d_gfx_t_use(node->t);
	for (int i = 0; i < node->num_meshes; i++) {
		d_draw_mesh_line(&node->meshes[i], c);
	}
	for (int i = 0; i < node->num_children; i++) {
		d_draw_model_node_line(&node->children[i], c);
	}
	d_gfx_t_pop();
}

void d_draw_model_line(d_model* model, color c) {
	d_gfx_t_push();
	for (int i = 0; i < model->num_nodes; i++) {
		d_draw_model_node_line(&model->nodes[i], c);
	}
	d_gfx_t_pop();
}

void d_gfx_set_blend(d_blend b) {
	d_gfx.blend = b;
}

void d_gfx_set_wrap(d_wrap w) {
	d_gfx.wrap = w;
}

void d_gfx_drawon(d_img* img) {
	if (img) {
		d_gfx.cur_canvas = img;
	} else {
		d_gfx.cur_canvas = &d_gfx.def_canvas;
	}
}

d_img* d_gfx_canvas() {
	return d_gfx.cur_canvas;
}

void d_mesh_gen_normals(d_mesh* mesh) {

	for (int i = 0; i < mesh->num_verts; i++) {
		mesh->verts[i].normal = vec3f(0, 0, 0);
	}

	for (int i = 0; i < mesh->num_indices; i += 3) {
		d_vertex* v1 = &mesh->verts[mesh->indices[i + 0]];
		d_vertex* v2 = &mesh->verts[mesh->indices[i + 1]];
		d_vertex* v3 = &mesh->verts[mesh->indices[i + 2]];
		vec3 n = vec3_cross(
			vec3_sub(v3->pos, v1->pos),
			vec3_sub(v2->pos, v1->pos)
		);
		v1->normal = vec3_add(v1->normal, n);
		v2->normal = vec3_add(v2->normal, n);
		v3->normal = vec3_add(v3->normal, n);
	}

	for (int i = 0; i < mesh->num_verts; i++) {
		mesh->verts[i].normal = vec3_unit(mesh->verts[i].normal);
	}

}

d_mesh d_mesh_new(
	d_vertex* verts,
	int num_verts,
	d_index* indices,
	int num_indices
) {

	int verts_size = sizeof(d_vertex) * num_verts;
	int indices_size = sizeof(d_index) * num_indices;

	d_vertex* verts2 = malloc(verts_size);
	d_index* indices2 = malloc(indices_size);

	memcpy(verts2, verts, verts_size);
	memcpy(indices2, indices, indices_size);

	return (d_mesh) {
		.verts = verts2,
		.num_verts = num_verts,
		.indices = indices2,
		.num_indices = num_indices,
	};

}

void d_mesh_free(d_mesh* mesh) {
	free(mesh->verts);
	free(mesh->indices);
	memset(mesh, 0, sizeof(d_mesh));
}

static void d_free_model_anim(d_model_anim* anim) {
	free(anim->frames);
	free(anim->name);
	memset(anim, 0, sizeof(d_model_anim));
}

static void d_free_model_node(d_model_node* node) {
	for (int i = 0; i < node->num_meshes; i++) {
		d_mesh_free(&node->meshes[i]);
	}
	for (int i = 0; i < node->num_children; i++) {
		d_free_model_node(&node->children[i]);
	}
	for (int i = 0; i < node->num_anims; i++) {
		d_free_model_anim(&node->anims[i]);
	}
	free(node->meshes);
	free(node->children);
	free(node->anims);
	memset(node, 0, sizeof(d_model_node));
}

static d_model d_model_empty() {
	return (d_model) {
		.nodes = malloc(0),
		.num_nodes = 0,
		.images = malloc(0),
		.num_images = 0,
		.bbox = boxf(vec3f(0, 0, 0), vec3f(0, 0, 0)),
		.center = vec3f(0, 0, 0),
	};
}

void d_free_model(d_model* model) {
	for (int i = 0; i < model->num_nodes; i++) {
		d_free_model_node(&model->nodes[i]);
	}
	for (int i = 0; i < model->num_images; i++) {
		d_img_free(&model->images[i]);
	}
	free(model->nodes);
	free(model->images);
	memset(model, 0, sizeof(d_model));
}

void d_draw_bbox(box bbox, color c) {
	vec3 p1 = vec3f(bbox.p1.x, bbox.p2.y, bbox.p1.z);
	vec3 p2 = vec3f(bbox.p2.x, bbox.p2.y, bbox.p1.z);
	vec3 p3 = vec3f(bbox.p2.x, bbox.p1.y, bbox.p1.z);
	vec3 p4 = bbox.p1;
	vec3 p5 = vec3f(bbox.p1.x, bbox.p2.y, bbox.p2.z);
	vec3 p6 = bbox.p2;
	vec3 p7 = vec3f(bbox.p2.x, bbox.p1.y, bbox.p2.z);
	vec3 p8 = vec3f(bbox.p1.x, bbox.p1.y, bbox.p2.z);
	d_draw_line3(p1, p2, c);
	d_draw_line3(p2, p3, c);
	d_draw_line3(p3, p4, c);
	d_draw_line3(p4, p1, c);
	d_draw_line3(p5, p6, c);
	d_draw_line3(p6, p7, c);
	d_draw_line3(p7, p8, c);
	d_draw_line3(p8, p5, c);
	d_draw_line3(p1, p5, c);
	d_draw_line3(p2, p6, c);
	d_draw_line3(p3, p7, c);
	d_draw_line3(p4, p8, c);
}

static void d_model_node_gen_bbox(
	d_model_node* node,
	box* bbox,
	mat4 t
) {
	t = mat4_mult(t, node->t);
	for (int i = 0; i < node->num_meshes; i++) {
		d_mesh* mesh = &node->meshes[i];
		for (int j = 0; j < mesh->num_verts; j++) {
			d_vertex* v = &mesh->verts[j];
			vec3 pos = mat4_mult_vec3(t, v->pos);
			bbox->p1.x = fminf(pos.x, bbox->p1.x);
			bbox->p1.y = fminf(pos.y, bbox->p1.y);
			bbox->p1.z = fminf(pos.z, bbox->p1.z);
			bbox->p2.x = fmaxf(pos.x, bbox->p2.x);
			bbox->p2.y = fmaxf(pos.y, bbox->p2.y);
			bbox->p2.z = fmaxf(pos.z, bbox->p2.z);
		}
	}
	for (int i = 0; i < node->num_children; i++) {
		d_model_node_gen_bbox(&node->children[i], bbox, t);
	}
}

void d_model_gen_bbox(d_model* model) {
	box bbox = boxf(vec3f(0, 0, 0), vec3f(0, 0, 0));
	for (int i = 0; i < model->num_nodes; i++) {
		d_model_node_gen_bbox(&model->nodes[i], &bbox, mat4u());
	}
	model->bbox = bbox;
	model->center = vec3_scale(vec3_add(bbox.p1, bbox.p2), 0.5);
}

static void d_assert(bool b, char* fmt, ...) {
	if (!b) {
		va_list args;
		va_start(args, fmt);
		vfprintf(stderr, fmt, args);
		va_end(args);
		exit(EXIT_FAILURE);
	}
}

#ifdef CGLTF_IMPLEMENTATION

static void d_model_parse_node(
	d_model* model,
	d_model_node* node,
	cgltf_node* cnode,
	cgltf_data* doc
) {

	memset(node, 0, sizeof(d_model_node));

	d_transform t = (d_transform) {
		.pos = vec3f(
			cnode->translation[0],
			cnode->translation[1],
			cnode->translation[2]
		),
		.rot = quatf(
			cnode->rotation[0],
			cnode->rotation[1],
			cnode->rotation[2],
			cnode->rotation[3]
		),
		.scale = vec3f(
			cnode->scale[0],
			cnode->scale[1],
			cnode->scale[2]
		),
	};

	node->t = d_transform_mat4(t);

	int num_children = cnode->children_count;

	node->num_children = num_children;
	node->children = malloc(sizeof(d_model_node) * num_children);

	for (int i = 0; i < num_children; i++) {
		d_model_parse_node(model, &node->children[i], cnode->children[i], doc);
	}

	if (!cnode->mesh) {
		node->num_meshes = 0;
		node->meshes = malloc(0);
		return;
	}

	int num_meshes = cnode->mesh->primitives_count;
	node->num_meshes = num_meshes;
	node->meshes = malloc(sizeof(d_mesh) * num_meshes);

	for (int i = 0; i < num_meshes; i++) {

		cgltf_primitive* prim = &cnode->mesh->primitives[i];
		int num_verts = prim->attributes[0].data->count;
		d_vertex* verts = calloc(num_verts, sizeof(d_vertex));

		for (int j = 0; j < num_verts; j++) {
			verts[j].color = colorx(0xffffffff);
		}

		for (int j = 0; j < prim->attributes_count; j++) {

			cgltf_attribute* attr = &prim->attributes[j];
			cgltf_accessor* acc = attr->data;

			d_assert(acc->count == num_verts, "bad gltf\n");

			switch (attr->type) {
				case cgltf_attribute_type_position:
					d_assert(
						acc->type == cgltf_type_vec3,
						"gltf pos must be vec3\n"
					);
					for(int k = 0; k < acc->count; k++) {
						cgltf_accessor_read_float(
							acc,
							k,
							(float*)&verts[k].pos,
							3
						);
					}
					break;
				case cgltf_attribute_type_normal:
					d_assert(
						acc->type == cgltf_type_vec3,
						"gltf normal must be vec3\n"
					);
					for(int k = 0; k < acc->count; k++) {
						cgltf_accessor_read_float(
							acc,
							k,
							(float*)&verts[k].normal,
							3
						);
					}
					break;
				case cgltf_attribute_type_texcoord:
					d_assert(
						acc->type == cgltf_type_vec2,
						"gltf texcoord must be vec2\n"
					);
					for(int k = 0; k < acc->count; k++) {
						cgltf_accessor_read_float(
							acc,
							k,
							(float*)&verts[k].uv,
							2
						);
					}
					break;
				case cgltf_attribute_type_color:
					d_assert(
						acc->type == cgltf_type_vec4,
						"gltf color must be vec4\n"
					);
					for(int k = 0; k < acc->count; k++) {
						cgltf_accessor_read_float(
							acc,
							k,
							(float*)&verts[k].color,
							4
						);
					}
					break;
				default:
					fprintf(stderr, "unknown gltf vert attrib: %d\n", attr->type);
					break;
			}

		}

		// TODO: some file have no indices
		int num_indices = prim->indices->count;
		d_index* indices = calloc(num_indices, sizeof(d_index));

		for (int j = 0; j < prim->indices->count; j++) {
			cgltf_accessor_read_uint(prim->indices, j, &indices[j], 1);
		}

		node->meshes[i] = (d_mesh) {
			.verts = verts,
			.num_verts = num_verts,
			.indices = indices,
			.num_indices = num_indices,
		};

		d_mesh_gen_normals(&node->meshes[i]);

	}

	node->num_anims = 0;
	node->anims = malloc(0);

	for (int i = 0; i < doc->animations_count; i++) {

		cgltf_animation* canim = &doc->animations[i];

		// TODO: squash same anims
		// TODO: UB around here?
		for (int j = 0; j < canim->channels_count; j++) {

			cgltf_animation_channel* chan = &canim->channels[j];

			if (chan->target_node == cnode) {

				node->anims = realloc(
					node->anims,
					sizeof(d_model_anim) * (node->num_anims + 1)
				);

				d_model_anim anim;
				memset(&anim, 0, sizeof(d_model_anim));
				anim.name = malloc(strlen(canim->name) + 1);
				strcpy(anim.name, canim->name);

				cgltf_animation_sampler* samp = chan->sampler;
				cgltf_accessor* frames_acc = samp->output;
				cgltf_accessor* time_acc = samp->input;

				d_assert(
					frames_acc->count == time_acc->count,
					"invalid gltf anim\n"
				);

				int num_frames = frames_acc->count;
				anim.num_frames = num_frames;
				anim.frames = malloc(num_frames * sizeof(d_model_anim_frame));

				d_assert(
					time_acc->type == cgltf_type_scalar,
					"gltf anim time must be scalar\n"
				);

				for (int k = 0; k < num_frames; k++) {
					cgltf_accessor_read_float(
						time_acc,
						k,
						&anim.frames[k].time,
						1
					);
					anim.frames[k].transform = mat4u();
				}

				switch (chan->target_path) {
					case cgltf_animation_path_type_translation:
						d_assert(
							frames_acc->type == cgltf_type_vec3,
							"gltf anim translation must be vec3\n"
						);
						for(int k = 0; k < num_frames; k++) {
							d_transform t2 = t;
							cgltf_accessor_read_float(
								frames_acc,
								k,
								(float*)&t2.pos,
								3
							);
							anim.frames[k].transform = d_transform_mat4(t2);
						}
						break;
					case cgltf_animation_path_type_rotation:
						d_assert(
							frames_acc->type == cgltf_type_vec4,
							"gltf anim rotation must be vec4\n"
						);
						for(int k = 0; k < num_frames; k++) {
							d_transform t2 = t;
							cgltf_accessor_read_float(
								frames_acc,
								k,
								(float*)&t2.rot,
								4
							);
							anim.frames[k].transform = d_transform_mat4(t2);
						}
						break;
					case cgltf_animation_path_type_scale:
						d_assert(
							frames_acc->type == cgltf_type_vec3,
							"gltf anim scale must be vec3\n"
						);
						for(int k = 0; k < num_frames; k++) {
							d_transform t2 = t;
							cgltf_accessor_read_float(
								frames_acc,
								k,
								(float*)&t2.scale,
								3
							);
							anim.frames[k].transform = d_transform_mat4(t2);
						}
						break;
					default:
						fprintf(stderr, "unknown gltf anim prop\n");
						break;
				}

				node->anims[node->num_anims++] = anim;

			}
		}
	}

}

static d_model d_model_parse_glb(uint8_t* bytes, int size) {

	cgltf_options options;
	memset(&options, 0, sizeof(cgltf_options));
	cgltf_data* doc = NULL;
	cgltf_result res = cgltf_parse(&options, bytes, size, &doc);
	cgltf_load_buffers(&options, doc, NULL);

	d_model model = d_model_empty();

	if (res != cgltf_result_success) {
		fprintf(stderr, "failed to parse gltf\n");
		return model;
	}

	int num_textures = doc->textures_count;
	model.num_images = num_textures;
	model.images = malloc(sizeof(d_img) * num_textures);

	for (int i = 0; i < num_textures; i++) {
		cgltf_image* img = doc->textures[i].image;
		cgltf_buffer_view* view = img->buffer_view;
		uint8_t* data = view->buffer->data;
		model.images[i] = d_img_parse(data + view->offset, view->size);
	}

	int num_nodes = doc->scene->nodes_count;
	model.num_nodes = num_nodes;
	model.nodes = malloc(sizeof(d_model_node) * num_nodes);

	for (int i = 0; i < num_nodes; i++) {
		d_model_parse_node(&model, &model.nodes[i], doc->scene->nodes[i], doc);
	}

	cgltf_free(doc);

	d_model_gen_bbox(&model);

	return model;

}

#endif // #ifdef CGLTF_IMPLEMENTATION

static uint8_t glb_sig[] = { 0x67, 0x6c, 0x54, 0x46 };

d_model d_model_parse(uint8_t* bytes, int size) {
	if (memcmp(bytes, glb_sig, sizeof(glb_sig)) == 0) {
#ifdef CGLTF_IMPLEMENTATION
		return d_model_parse_glb(bytes, size);
#else
		fprintf(stderr, "glb support requires 'cgltf.h'\n");
		return d_model_empty();
#endif
	} else {
		fprintf(stderr, "unsupported model format\n");
		return d_model_empty();
	}
}

#ifdef D_FS_H
d_model d_model_load(char* path) {
	size_t size;
	uint8_t* bytes = d_read_bytes(path, &size);
	if (!bytes) {
		fprintf(stderr, "failed to load model from '%s'\n", path);
		return d_model_empty();
	}
	d_model model = d_model_parse(bytes, size);
	free(bytes);
	return model;
}
#endif // #ifdef D_FS_H

#endif // #ifndef D_GFX_IMPL_ONCE
#endif // #ifdef D_GFX_IMPL
