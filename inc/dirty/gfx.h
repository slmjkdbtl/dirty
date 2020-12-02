// wengwengweng

#ifndef D_GFX_H
#define D_GFX_H

#define D_ASCII_CHARS " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"

#include <stdbool.h>

#include "math.h"

typedef enum {
	D_CENTER,
	D_TOP_LEFT,
	D_TOP,
	D_TOP_RIGHT,
	D_LEFT,
	D_RIGHT,
	D_BOT_LEFT,
	D_BOT,
	D_BOT_RIGHT,
} d_origin;

typedef enum {
	D_NEAREST,
	D_LINEAR,
} d_img_filter;

typedef enum {
	D_REPEAT,
	D_MIRRORED_REPEAT,
	D_CLAMP_TO_EDGE,
	D_CLAMP_TO_BORDER,
} d_img_wrap;

typedef enum {
	D_BLEND_ALPHA,
	D_BLEND_ADD,
	D_BLEND_REPLACE,
} d_blend;

// base vertex type
typedef struct {
	vec3 pos;
	vec3 normal;
	vec2 uv;
	color color;
} d_vertex;

// base index type
typedef unsigned int d_index;

// a camera with projection and view matrices
typedef struct {
	mat4 view;
	mat4 proj;
} d_cam;

typedef struct {
	vec3 pos;
	vec3 scale;
	quat rot;
} d_psr;

// OpenGL shader
typedef struct {
	unsigned int id;
} d_shader;

// image buffer
typedef struct {
	unsigned char *pixels;
	int width;
	int height;
} d_img_data;

// OpenGL texture
typedef struct {
	unsigned int id;
	int width;
	int height;
} d_img;

// bitmap font
typedef struct {
	d_img img;
	int height;
	int width;
	float qw;
	float qh;
	vec2 map[128];
} d_font;

typedef struct {
	d_vertex *verts;
	int num_verts;
	d_index *indices;
	int num_indices;
} d_mesh_data;

// OpenGL vertex / index buffer
typedef struct {
	unsigned int vbuf;
	unsigned int ibuf;
	int count;
} d_mesh;

typedef struct d_model_node_data {
	d_psr psr;
	struct d_model_node_data *children;
	int num_children;
	d_mesh_data *meshes;
	int num_meshes;
} d_model_node_data;

typedef struct {
	d_model_node_data *nodes;
	int num_nodes;
	d_img_data *images;
	int num_images;
} d_model_data;

typedef struct d_model_node {
	d_psr psr;
	struct d_model_node *children;
	int num_children;
	d_mesh *meshes;
	int num_meshes;
} d_model_node;

typedef struct {
	d_model_node *nodes;
	int num_nodes;
	d_img *images;
	int num_images;
	box bbox;
} d_model;

// OpenGL framebuffer
typedef struct {
	unsigned int fbuf;
	d_img cimg;
	unsigned int dsimg;
	int width;
	int height;
} d_canvas;

// texture creation config
typedef struct {
	d_img_filter filter;
	d_img_wrap wrap;
} d_img_conf;

// formated character
typedef struct {
	vec2 pos;
	color color;
	quad quad;
} d_fchar;

// formated text
typedef struct {
	float width;
	float height;
	const d_img *img;
	float scale;
	int len;
	float cw;
	float ch;
	d_origin origin;
	d_fchar chars[256];
} d_ftext;

// text chunk
typedef struct {
	const char *text;
	color color;
	vec2 offset;
} d_textc;

typedef struct {
	const char *name;
	int from;
	int to;
} d_sprite_anim;

typedef struct {
	quad *frames;
	d_sprite_anim *anims;
	int num_frames;
	int num_anims;
	float w;
	float h;
} d_sprite_data;

// mesh
box d_mesh_data_bbox(const d_mesh_data *data);
void d_free_mesh_data(d_mesh_data *data);
d_mesh d_make_mesh(const d_mesh_data *data);
void d_free_mesh(d_mesh *mesh);

// image
d_img_data d_parse_img_data(const unsigned char *bytes, int size);
d_img_data d_load_img_data(const char *path);
void d_img_data_save(d_img_data *img, const char *path);
void d_free_img_data(d_img_data *img);

// image
d_img d_make_img(const d_img_data *img);
d_img d_make_img_ex(const d_img_data *img, d_img_conf conf);
d_img d_parse_img(const unsigned char *bytes, int size);
d_img d_parse_img_ex(const unsigned char *bytes, int size, d_img_conf conf);
d_img d_load_img(const char *path);
d_img d_load_img_ex(const char *path, d_img_conf conf);
void d_free_img(d_img *img);

// model
d_model_data d_parse_model_data(const unsigned char *bytes, int size);
d_model_data d_load_model_data(const char *path);
void d_free_model_data(d_model_data *data);
d_model d_make_model(const d_model_data *data);
d_model d_parse_model(const unsigned char *bytes, int size);
d_model d_load_model(const char *path);
void d_free_model(d_model *model);

// font
d_font d_make_font(d_img img, int grid_width, int grid_height, const char *chars);
void d_free_font(d_font *font);

// shader
d_shader d_make_shader(const char *vs_src, const char *fs_src);
d_shader d_load_shader(const char *vs_path, const char *fs_path);
void d_free_shader(d_shader *shader);

// canvas
d_canvas d_make_canvas(int width, int height);
d_canvas d_make_canvas_ex(int, int, d_img_conf);
d_img_data d_canvas_capture(const d_canvas *canvas);
void d_free_canvas(d_canvas *canvas);

// send uniform values
void d_send_f(const char *name, float val);
void d_send_vec2(const char *name, vec2 val);
void d_send_vec3(const char *name, vec3 val);
void d_send_color(const char *name, color val);
void d_send_mat4(const char *name, mat4 val);
// send uniform img to specified slot
void d_send_img(const char *name, int slot, const d_img *img);

// clear all buffers
void d_clear();
// clear specified buffer
void d_clear_color();
void d_clear_depth();
void d_clear_stencil();

// enable / set OpenGL states
void d_depth_write(bool b);
void d_depth_test(bool b);
void d_stencil_write(bool b);
void d_stencil_test(bool b);
void d_blend_mode(d_blend b);

// get the position of a window coord
vec2 d_coord(d_origin orig);
vec2 d_origin_pt(d_origin orig);
// get current transform
mat4 d_transform();
// get mouse position relative to current transform
vec2 d_mouse_pos_t();

mat4 d_psr_mat4(d_psr psr);

// push transform stack
void d_push();
// pop transform stack
void d_pop();
// translate
void d_move(vec3 pos);
void d_move_x(float x);
void d_move_y(float y);
void d_move_z(float z);
void d_move_xy(vec2 pos);
// scale
void d_scale(vec3 pos);
void d_scale_x(float x);
void d_scale_y(float y);
void d_scale_z(float z);
void d_scale_xy(vec2 pos);
// rotate
void d_rot_x(float angle);
void d_rot_y(float angle);
void d_rot_z(float angle);

// set current binded object
void d_use_cam(const d_cam *cam);
void d_use_shader(const d_shader *shader);
void d_use_font(const d_font *font);
void d_use_canvas(const d_canvas *canvas);

// query current state
const d_font *d_cur_font();

// draw
void d_draw_raw(const d_vertex *vertices, int num_verts, const d_index *indices, int num_indices, const d_img *img);
void d_draw_mesh(const d_mesh *mesh);
void d_draw_model(const d_model *model);
void d_draw_img(const d_img *img, quad q, color c);
void d_draw_text(const char *txt, float size, float wrap, d_origin orig, color c);
void d_draw_ftext(const d_ftext *ftext);
void d_draw_canvas(const d_canvas *canvas, color c);
void d_draw_rect(vec2 p1, vec2 p2, color c);
void d_draw_lrect(vec2 p1, vec2 p2, float width, color c);
void d_draw_line(vec2 p1, vec2 p2, float width, color c);
void d_draw_circle(vec2 center, float radius, color c);

// format text
d_ftext d_fmt_text(const char *txt, float size, float wrap, d_origin orig, color c);
d_ftext d_fmt_textc(const d_textc *chunks, int count, float size, float wrap, d_origin orig);
// get cursor position of formatted text
vec2 d_ftext_cpos(const d_ftext *ftext, int cursor);

// sprite data
d_sprite_data d_parse_ase(const char *json);
d_sprite_data d_load_ase(const char *path);
void d_free_sprite_data(d_sprite_data *data);

int d_draw_calls();
const char *d_gl_version();
const char *d_glsl_version();
const char *d_gl_renderer();
const char *d_gl_vendor();

#endif

