// wengwengweng

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "math.h"
#include "gl.h"
#include "gfx.h"
#include "event.h"
#include "app.h"
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

d_gfx_t d_gfx;

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

	// test mesh
	d_vertex verts[] = {
		{
			.pos = { 0.0, 120.0, 0.0, },
			.normal = { 0.0, 0.0, 1.0 },
			.uv = { 0.0, 0.0, },
			.color = { 1.0, 0.0, 0.0, 1.0, }
		},
		{
			.pos = { -160.0, -120.0, 0.0, },
			.normal = { 0.0, 0.0, 1.0 },
			.uv = { 0.0, 0.0, },
			.color = { 0.0, 1.0, 0.0, 1.0, }
		},
		{
			.pos = { 160.0, -120.0, 0.0, },
			.normal = { 0.0, 0.0, 1.0 },
			.uv = { 0.0, 0.0, },
			.color = { 0.0, 0.0, 1.0, 1.0, }
		},
	};

	unsigned int indices[] = {
		0, 1, 2,
	};

	d_gfx.tri_mesh = d_make_mesh(verts, sizeof(verts), indices, sizeof(indices));

	// init default program
	d_gfx.default_prog = d_make_program(NULL, NULL);
	d_gfx.cur_prog = &d_gfx.default_prog;

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
	d_gfx.default_cam.view = make_mat4();
	d_gfx.default_cam.proj = mat4_ortho(d_app.width, d_app.height, -1024.0, 1024.0);
	d_gfx.cur_cam = &d_gfx.default_cam;

	// init transform
	d_gfx.transform = make_mat4();

}

void d_gfx_frame_start() {
	d_gfx.transform = make_mat4();
}

d_mesh d_make_mesh(const d_vertex* verts, size_t verts_size, const unsigned int* indices, size_t indices_size) {

	// vertex buffer
	GLuint vbuf;

	glGenBuffers(1, &vbuf);
	glBindBuffer(GL_ARRAY_BUFFER, vbuf);
	glBufferData(GL_ARRAY_BUFFER, verts_size, verts, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// index buffer
	GLuint ibuf;

	glGenBuffers(1, &ibuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return (d_mesh) {
		.vbuf = vbuf,
		.ibuf = ibuf,
		.count = indices_size / sizeof(unsigned int),
	};

}

void d_free_mesh(d_mesh* m) {
	glDeleteBuffers(1, &m->vbuf);
	glDeleteBuffers(1, &m->ibuf);
	m->vbuf = 0;
	m->ibuf = 0;
}

d_img d_make_img(const unsigned char* data, int w, int h) {

	size_t size = w * h * 4;
	unsigned char* mdata = malloc(size);
	memcpy(mdata, data, size);

	return (d_img) {
		.data = mdata,
		.width = w,
		.height = h,
	};
}

d_img d_parse_img(const unsigned char* bytes, size_t len) {

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
		d_quit();
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

	char* res = (char*)malloc(i + cnt * (new_len - old_len) + 1);
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

d_program d_make_program(const char* vs_src, const char* fs_src) {

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
		d_quit();
	}

	// fragment shader
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fs, 1, &fs_code, 0);
	glCompileShader(fs);

	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE) {
		glGetShaderInfoLog(fs, 512, NULL, info_log);
		fprintf(stderr, "%s", info_log);
		d_quit();
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
		d_quit();
	}

	free((void*)vs_code);
	free((void*)fs_code);

	return (d_program) {
		.id = program,
	};

}

void d_free_program(d_program* p) {
	glDeleteProgram(p->id);
	p->id = 0;
}

void d_send_f(const char* name, float v) {
	glUniform1f(glGetUniformLocation(d_gfx.cur_prog->id, name), v);
}

void d_send_vec2(const char* name, vec2 v) {
	glUniform2f(glGetUniformLocation(d_gfx.cur_prog->id, name), v.x, v.y);
}

void d_send_vec3(const char* name, vec3 v) {
	glUniform3f(glGetUniformLocation(d_gfx.cur_prog->id, name), v.x, v.y, v.z);
}

void d_send_color(const char* name, color c) {
	glUniform4f(glGetUniformLocation(d_gfx.cur_prog->id, name), c.r, c.g, c.b, c.a);
}

void d_send_mat4(const char* name, mat4 m) {
	glUniformMatrix4fv(glGetUniformLocation(d_gfx.cur_prog->id, name), 1, GL_FALSE, &m.m[0]);
}

void d_draw(d_mesh* mesh, d_program* program) {

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibuf);
	glUseProgram(program->id);

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
	d_send_color("u_color", (color) { 1.0, 1.0, 1.0, 1.0, });

	glDrawElements(GL_TRIANGLES, mesh->count, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	for (int i = 0; d_gfx.tex_slots[i] != NULL; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glUseProgram(0);

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
		return d_quit();
	}

	d_gfx.t_stack[d_gfx.t_stack_cnt] = d_gfx.transform;
	d_gfx.t_stack_cnt++;

}

void d_pop() {

	if (d_gfx.t_stack_cnt <= 0) {
		fprintf(stderr, "transform stack underflow");
		return d_quit();
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

