// wengwengweng

#ifndef D_GFX_INTERNAL_H
#define D_GFX_INTERNAL_H

#define BATCH_VERT_COUNT 65536
#define BATCH_INDEX_COUNT 65536

static const char *d_vert_template =
#ifndef GLES
"#version 120\n"
#endif
"attribute vec3 a_pos;"
"attribute vec3 a_normal;"
"attribute vec2 a_uv;"
"attribute vec4 a_color;"
"varying vec3 v_pos;"
"varying vec3 v_normal;"
"varying vec2 v_uv;"
"varying vec4 v_color;"
"uniform mat4 u_model;"
"uniform mat4 u_view;"
"uniform mat4 u_proj;"
"vec4 default_pos() {"
	"return u_proj * u_view * u_model * vec4(v_pos, 1.0);"
"}"
"##USER##"
"void main() {"
	"v_pos = a_pos;"
	"v_uv = a_uv;"
	"v_color = a_color;"
	"v_normal = normalize(a_normal);"
	"gl_Position = vert();"
"}"
;

static const char *d_frag_template =
#ifndef GLES
"#version 120\n"
#else
"precision mediump float;"
#endif
"varying vec3 v_pos;"
"varying vec3 v_normal;"
"varying vec2 v_uv;"
"varying vec4 v_color;"
"uniform sampler2D u_tex;"
"uniform vec4 u_color;"
"vec4 default_color() {"
	"return v_color * u_color * texture2D(u_tex, v_uv);"
"}"
"##USER##"
"void main() {"
	"gl_FragColor = frag();"
	"if (gl_FragColor.a == 0.0) {"
		"discard;"
	"}"
"}"
;

static const char *d_vert_default =
"vec4 vert() {"
	"return default_pos();"
"}"
;

static const char *d_frag_default =
"vec4 frag() {"
	"return default_color();"
"}"
;

typedef struct {
	GLuint vbuf;
	GLuint ibuf;
	d_vertex vqueue[BATCH_VERT_COUNT];
	d_index iqueue[BATCH_INDEX_COUNT];
	int vcount;
	int icount;
} d_batch;

void d_gfx_init();
void d_gfx_frame_end();

void d_gl_check_errors();
void d_draw(GLuint, GLuint, int);
d_batch d_make_batch();
void d_batch_push(d_batch*, const d_vertex*, int, const d_index*, int);
void d_batch_flush(d_batch*);
void d_free_batch(d_batch*);

#endif

