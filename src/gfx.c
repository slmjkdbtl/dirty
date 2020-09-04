// wengwengweng

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <dirty/dirty.h>
#include "gfx.h"
#include "res/unscii.png.h"

static const char* vert_template =
"#version 120\n"
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

static const char* frag_template =
"#version 120\n"
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

static const char* vert_default =
"vec4 vert() {"
	"return default_pos();"
"}"
;

static const char* frag_default =
"vec4 frag() {"
	"return default_color();"
"}"
;

typedef struct {
	d_tex2d default_tex;
	d_tex2d* tex_slots[4];
	d_font default_font;
	const d_font* cur_font;
	d_shader default_shader;
	const d_shader* cur_shader;
	mat4 transform;
	d_cam default_cam;
	const d_cam* cur_cam;
	mat4 t_stack[8];
	int t_stack_cnt;
	d_batch batch;
} d_gfx_t;

static d_gfx_t d_gfx;

void d_gfx_init() {

	// init gl
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glStencilFunc(GL_EQUAL, 1, 0xff);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

// 	const GLubyte* gl_ver = glGetString(GL_VERSION);
// 	printf("%s\n", gl_ver);

	// init default shader
	d_gfx.default_shader = d_make_shader(NULL, NULL);
	d_gfx.cur_shader = &d_gfx.default_shader;

	// init default tex
	d_img default_img = d_make_img((unsigned char[]){255, 255, 255, 255}, 1, 1);
	d_gfx.default_tex = d_make_tex(&default_img);
	d_free_img(&default_img);
	d_gfx.tex_slots[0] = &d_gfx.default_tex;

	// init default font
	d_img font_img = d_parse_img(unscii_png, unscii_png_len);
	d_gfx.default_font = d_make_font(d_make_tex(&font_img), 8, 8, " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");
	d_free_img(&font_img);
	d_gfx.cur_font = &d_gfx.default_font;

	// init default cam
	d_gfx.default_cam.view = mat4u();
	d_gfx.default_cam.proj = mat4_ortho(d_width(), d_height(), -1024.0, 1024.0);
	d_gfx.cur_cam = &d_gfx.default_cam;

	d_gfx.batch = d_make_batch();

	// init transform
	d_gfx.transform = mat4u();

}

d_mesh d_make_mesh(
	const d_vertex* verts,
	int vcount,
	const d_index* indices,
	int icount
) {

	// vertex buffer
	GLuint vbuf;

	glGenBuffers(1, &vbuf);
	glBindBuffer(GL_ARRAY_BUFFER, vbuf);
	glBufferData(GL_ARRAY_BUFFER, vcount * sizeof(d_vertex), verts, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// index buffer
	GLuint ibuf;

	glGenBuffers(1, &ibuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, icount * sizeof(d_index), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return (d_mesh) {
		.vbuf = vbuf,
		.ibuf = ibuf,
		.count = icount,
	};

}

void d_free_mesh(d_mesh* m) {
	glDeleteBuffers(1, &m->vbuf);
	glDeleteBuffers(1, &m->ibuf);
	m->vbuf = 0;
	m->ibuf = 0;
}

d_batch d_make_batch() {

	// vertex buffer
	GLuint vbuf;

	glGenBuffers(1, &vbuf);
	glBindBuffer(GL_ARRAY_BUFFER, vbuf);
	glBufferData(GL_ARRAY_BUFFER, BATCH_VERT_COUNT * sizeof(d_vertex), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// index buffer
	GLuint ibuf;

	glGenBuffers(1, &ibuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, BATCH_INDEX_COUNT * sizeof(d_index), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return (d_batch) {
		.vbuf = vbuf,
		.ibuf = ibuf,
		.vqueue = {},
		.iqueue = {},
		.vcount = 0,
		.icount = 0,
	};

}

void d_batch_flush(d_batch* m) {

	glBindBuffer(GL_ARRAY_BUFFER, m->vbuf);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m->vcount * sizeof(d_vertex), &m->vqueue);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ibuf);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m->icount * sizeof(d_index), &m->iqueue);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	d_draw(m->vbuf, m->ibuf, m->icount);

	m->vcount = 0;
	m->icount = 0;
	memset(m->vqueue, 0, sizeof(m->vqueue));
	memset(m->iqueue, 0, sizeof(m->iqueue));

}

void d_batch_push(
	d_batch* m,
	const d_vertex* verts,
	int vcount,
	const d_index* indices,
	int icount
) {

	if (m->vcount + vcount >= BATCH_VERT_COUNT || m->icount + icount >= BATCH_INDEX_COUNT) {
		d_batch_flush(m);
	}

	memcpy(&m->vqueue[m->vcount], verts, vcount * sizeof(d_vertex));
	memcpy(&m->iqueue[m->vcount], indices, icount * sizeof(d_index));
	m->vcount += vcount;
	m->icount += icount;

}

void d_free_batch(d_batch* m) {
	glDeleteBuffers(1, &m->vbuf);
	glDeleteBuffers(1, &m->ibuf);
	m->vbuf = 0;
	m->ibuf = 0;
}

d_img d_make_img(const unsigned char* data, int w, int h) {

	int size = w * h * 4;
	unsigned char* mdata = malloc(size);
	memcpy(mdata, data, size);

	return (d_img) {
		.data = mdata,
		.width = w,
		.height = h,
	};
}

d_img d_parse_img(const unsigned char* bytes, int len) {

	int w, h;
	unsigned char *data = stbi_load_from_memory((unsigned char*)bytes, len, &w, &h, NULL, 0);

	return (d_img) {
		.data = data,
		.width = w,
		.height = h,
	};

}

void d_free_img(d_img* img) {
	free(img->data);
	img->data = 0;
}

d_tex2d d_make_tex(const d_img* img) {

	GLuint tex;

	glGenTextures(1, &tex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		img->width,
		img->height,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		img->data
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);

	return (d_tex2d) {
		.id = tex,
		.width = img->width,
		.height = img->height,
	};

}

void d_free_tex(d_tex2d* t) {
	glDeleteTextures(1, &t->id);
	t->id = 0;
}

d_font d_make_font(d_tex2d tex, int gw, int gh, const char* chars) {

	d_font f = {};

	int cols = tex.width / gw;
	int rows = tex.height / gh;
	int count = cols * rows;

	f.qw = 1.0 / cols;
	f.qh = 1.0 / rows;
	f.tex = tex;

	if (count != strlen(chars)) {
		fprintf(stderr, "invalid font");
		d_quit(EXIT_FAILURE);
	}

	for (int i = 0; i < count; i++) {
		f.map[(int)chars[i]] = (vec2) {
			.x = (i % cols) * f.qw,
			.y = (i / cols) * f.qh,
		};
	}

	return f;

}

void d_free_font(d_font* f) {
	d_free_tex(&f->tex);
}

char* strsub(const char* str, const char* old, const char* new) {

	int old_len = strlen(old);
	int new_len = strlen(new);
	int i = 0;
	int cnt = 0;

	for (i = 0; str[i] != '\0'; i++) {
		if (strstr(&str[i], old) == &str[i]) {
			cnt++;
			i += old_len - 1;
		}
	}

	char* res = malloc(i + cnt * (new_len - old_len) + 1);
	i = 0;

	while (*str) {
		if (strstr(str, old) == str) {
			strcpy(&res[i], new);
			i += new_len;
			str += old_len;
		} else {
			res[i++] = *str++;
		}
	}

	res[i] = '\0';

	return res;

}

d_shader d_make_shader(const char* vs_src, const char* fs_src) {

	if (vs_src == NULL) {
		vs_src = vert_default;
	}

	if (fs_src == NULL) {
		fs_src = frag_default;
	}

	const char* vs_code = strsub(vert_template, "##USER##", vs_src);
	const char* fs_code = strsub(frag_template, "##USER##", fs_src);

	GLchar info_log[512];
	GLint success = 0;

	// vertex shader
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vs, 1, &vs_code, 0);
	glCompileShader(vs);

	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE) {
		glGetShaderInfoLog(vs, 512, NULL, info_log);
		fprintf(stderr, "%s", info_log);
		d_quit(EXIT_FAILURE);
	}

	// fragment shader
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fs, 1, &fs_code, 0);
	glCompileShader(fs);

	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE) {
		glGetShaderInfoLog(fs, 512, NULL, info_log);
		fprintf(stderr, "%s", info_log);
		d_quit(EXIT_FAILURE);
	}

	// program
	GLuint program = glCreateProgram();

	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glBindAttribLocation(program, 0, "a_pos");
	glBindAttribLocation(program, 1, "a_normal");
	glBindAttribLocation(program, 2, "a_uv");
	glBindAttribLocation(program, 3, "a_color");

	glLinkProgram(program);

	glDetachShader(program, vs);
	glDetachShader(program, fs);
	glDeleteShader(vs);
	glDeleteShader(fs);

	glGetProgramiv(program, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE) {
		glGetProgramInfoLog(program, 512, NULL, info_log);
		fprintf(stderr, "%s", info_log);
		d_quit(EXIT_FAILURE);
	}

	free((void*)vs_code);
	free((void*)fs_code);

	return (d_shader) {
		.id = program,
	};

}

void d_free_shader(d_shader* p) {
	glDeleteProgram(p->id);
	p->id = 0;
}

void d_send_f(const char* name, float v) {
	glUniform1f(glGetUniformLocation(d_gfx.cur_shader->id, name), v);
}

void d_send_vec2(const char* name, vec2 v) {
	glUniform2f(glGetUniformLocation(d_gfx.cur_shader->id, name), v.x, v.y);
}

void d_send_vec3(const char* name, vec3 v) {
	glUniform3f(glGetUniformLocation(d_gfx.cur_shader->id, name), v.x, v.y, v.z);
}

void d_send_color(const char* name, color c) {
	glUniform4f(glGetUniformLocation(d_gfx.cur_shader->id, name), c.r, c.g, c.b, c.a);
}

void d_send_mat4(const char* name, mat4 m) {
	glUniformMatrix4fv(glGetUniformLocation(d_gfx.cur_shader->id, name), 1, GL_FALSE, &m.m[0]);
}

void d_clear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void d_clear_color() {
	glClear(GL_COLOR_BUFFER_BIT);
}

void d_clear_depth() {
	glClear(GL_DEPTH_BUFFER_BIT);
}

void d_clear_stencil() {
	glClear(GL_STENCIL_BUFFER_BIT);
}

void d_depth_write(bool b) {
	glDepthMask(b);
}

void d_depth_test(bool b) {
	if (b) {
		glEnable(GL_DEPTH_TEST);
	} else {
		glDisable(GL_DEPTH_TEST);
	}
}

void d_stencil_write(bool b) {
	if (b) {
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	} else {
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	}
}

void d_stencil_test(bool b) {
	if (b) {
		glEnable(GL_STENCIL_TEST);
	} else {
		glDisable(GL_STENCIL_TEST);
	}
}

void d_push() {

	if (d_gfx.t_stack_cnt >= 16) {
		fprintf(stderr, "transform stack overflow");
		d_quit(EXIT_FAILURE);
	}

	d_gfx.t_stack[d_gfx.t_stack_cnt] = d_gfx.transform;
	d_gfx.t_stack_cnt++;

}

void d_pop() {

	if (d_gfx.t_stack_cnt <= 0) {
		fprintf(stderr, "transform stack underflow");
		d_quit(EXIT_FAILURE);
	}

	d_gfx.t_stack_cnt--;
	d_gfx.transform = d_gfx.t_stack[d_gfx.t_stack_cnt];

}

void d_move(vec3 p) {
	d_gfx.transform = mat4_mult(d_gfx.transform, mat4_translate(p));
}

void d_scale(vec3 s) {
	d_gfx.transform = mat4_mult(d_gfx.transform, mat4_scale(s));
}

void d_rot_x(float a) {
	d_gfx.transform = mat4_mult(d_gfx.transform, mat4_rot_x(a));
}

void d_rot_y(float a) {
	d_gfx.transform = mat4_mult(d_gfx.transform, mat4_rot_y(a));
}

void d_rot_z(float a) {
	d_gfx.transform = mat4_mult(d_gfx.transform, mat4_rot_z(a));
}

void d_use_cam(const d_cam* cam) {
	if (cam) {
		d_gfx.cur_cam = cam;
	} else {
		d_gfx.cur_cam = &d_gfx.default_cam;
	}
}

void d_use_shader(const d_shader* shader) {
	if (shader) {
		d_gfx.cur_shader = shader;
	} else {
		d_gfx.cur_shader = &d_gfx.default_shader;
	}
}

void d_use_font(const d_font* font) {
	if (font) {
		d_gfx.cur_font = font;
	} else {
		d_gfx.cur_font = &d_gfx.default_font;
	}
}

void d_draw(GLuint vbuf, GLuint ibuf, int count) {

	glBindBuffer(GL_ARRAY_BUFFER, vbuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuf);
	glUseProgram(d_gfx.cur_shader->id);

	for (int i = 0; d_gfx.tex_slots[i] != NULL; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, d_gfx.tex_slots[i]->id);
	}

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 48, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 48, (void*)12);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 48, (void*)24);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 48, (void*)32);
	glEnableVertexAttribArray(3);

	d_send_mat4("u_model", d_gfx.transform);
	d_send_mat4("u_view", d_gfx.cur_cam->view);
	d_send_mat4("u_proj", d_gfx.cur_cam->proj);
	d_send_color("u_color", coloru());

	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	for (int i = 0; d_gfx.tex_slots[i] != NULL; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glUseProgram(0);

}

void d_draw_mesh(const d_mesh* mesh) {
	d_draw(mesh->vbuf, mesh->ibuf, mesh->count);
}

void d_draw_raw(
	const d_vertex* verts,
	int vcount,
	const d_index* indices,
	int icount
) {
	d_batch_push(&d_gfx.batch, verts, vcount, indices, icount);
}

void d_gfx_frame_start() {
	d_gfx.transform = mat4u();
	d_gfx.default_cam.proj = mat4_ortho(d_width(), d_height(), -1024.0, 1024.0);
	d_gfx.cur_cam = &d_gfx.default_cam;
	d_clear();
}

void d_gfx_frame_end() {
	d_batch_flush(&d_gfx.batch);
}

