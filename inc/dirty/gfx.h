// wengwengweng

#ifndef D_GFX_H
#define D_GFX_H

#include <stdbool.h>

#include "app.h"
#include "math.h"
#include "gl.h"

typedef enum {
	D_TOP_LEFT,
	D_TOP,
	D_TOP_RIGHT,
	D_LEFT,
	D_CENTER,
	D_RIGHT,
	D_BOT_LEFT,
	D_BOT,
	D_BOT_RIGHT,
} d_origin;

typedef enum {
	D_NEAREST = GL_NEAREST,
	D_LINEAR = GL_LINEAR,
} d_tex_filter;

typedef enum {
	D_REPEAT = GL_REPEAT,
	D_MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
	D_CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
#ifndef GLES
	D_CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER,
#endif
} d_tex_wrap;

typedef enum {
	D_BLEND_ALPHA,
	D_BLEND_ADD,
	D_BLEND_REPLACE,
} d_blend;

// a camera with projection and view matrices
typedef struct {
	mat4 view;
	mat4 proj;
} d_cam;

// OpenGL shader
typedef struct {
	GLuint id;
} d_shader;

// image buffer
typedef struct {
	unsigned char *data;
	int width;
	int height;
} d_img;

// OpenGL texture
typedef struct {
	GLuint id;
	int width;
	int height;
} d_tex;

// bitmap font
typedef struct {
	d_tex tex;
	float qw;
	float qh;
	vec2 map[128];
} d_font;

// OpenGL vertex / index buffer
typedef struct {
	GLuint vbuf;
	GLuint ibuf;
	int count;
} d_mesh;

typedef struct d_model {
	d_mesh *meshes;
	int num_meshes;
	struct d_model *children;
	int num_children;
} d_model;

// OpenGL framebuffer
typedef struct {
	GLuint fbuf;
	d_tex ctex;
	GLuint dstex;
	int width;
	int height;
} d_canvas;

// texture creation config
typedef struct {
	d_tex_filter filter;
	d_tex_wrap wrap;
} d_tex_conf;

// base vertex type
typedef struct {
	vec3 pos;
	vec3 normal;
	vec2 uv;
	color color;
} d_vertex;

// base index type
typedef unsigned int d_index;

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
	const d_tex *tex;
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
d_mesh d_make_mesh(const d_vertex *vertices, int vcount, const d_index *indices, int icount);
void d_free_mesh(d_mesh *mesh);

// image
d_img d_make_img(const unsigned char *pixels, int width, int height);
d_img d_parse_img(const unsigned char *bytes, int size);
d_img d_load_img(const char *path);
void d_img_write(d_img *img, const char *path);
void d_free_img(d_img *img);

// texture
d_tex d_img_tex(const d_img *img);
d_tex d_img_tex_ex(const d_img *img, d_tex_conf conf);
d_tex d_make_tex(const unsigned char *pixels, int width, int height);
d_tex d_make_tex_ex(const unsigned char *pixels, int width, int height, d_tex_conf conf);
d_tex d_parse_tex(const unsigned char *bytes, int size);
d_tex d_parse_tex_ex(const unsigned char *bytes, int size, d_tex_conf conf);
d_tex d_load_tex(const char *path);
d_tex d_load_tex_ex(const char *path, d_tex_conf conf);
void d_free_tex(d_tex *tex);

d_model d_parse_model(const unsigned char *bytes, int size);
d_model d_load_model(const char *path);
void d_free_model(d_model *model);

// font
d_font d_make_font(d_tex tex, int grid_width, int grid_height, const char *chars);
void d_free_font(d_font *font);

// shader
d_shader d_make_shader(const char *vs_src, const char *fs_src);
d_shader d_load_shader(const char *vs_path, const char *fs_path);
void d_free_shader(d_shader *shader);

// canvas
d_canvas d_make_canvas(int width, int height);
d_canvas d_make_canvas_ex(int, int, d_tex_conf);
#ifndef GLES
d_img d_canvas_capture(const d_canvas *canvas);
#endif
void d_free_canvas(d_canvas *canvas);

// send uniform values
void d_send_f(const char *name, float val);
void d_send_vec2(const char *name, vec2 val);
void d_send_vec3(const char *name, vec3 val);
void d_send_color(const char *name, color val);
void d_send_mat4(const char *name, mat4 val);
// send uniform tex to specified slot
void d_send_tex(const char *name, int slot, const d_tex *tex);

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

// draw
void d_draw_raw(const d_vertex *vertices, int vcount, const d_index *indices, int icount, const d_tex *tex);
void d_draw_mesh(const d_mesh *mesh);
void d_draw_tex(const d_tex *tex, quad q, color c);
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

const char *d_gl_version();
const char *d_glsl_version();
const char *d_gl_renderer();
const char *d_gl_vendor();

#endif

