// wengwengweng

#include <ctype.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>
#define CGLTF_IMPLEMENTATION
#include <cgltf/cgltf.h>
#include <json.h/json.h>

#include <dirty/dirty.h>

#include "gfx.h"
#include "utils.h"
// http://pelulamu.net/unscii/
#include "res/unscii.png.h"

#define T_STACKS 16
#define TEX_SLOTS 4
#define NEAR -1024.0
#define FAR 1024.0

typedef struct {
	d_tex default_tex;
	const d_tex *tex_slots[TEX_SLOTS];
	d_font default_font;
	const d_font *cur_font;
	d_shader default_shader;
	const d_shader *cur_shader;
	mat4 transform;
	d_cam default_cam;
	const d_cam *cur_cam;
	const d_canvas *cur_canvas;
	mat4 t_stack[T_STACKS];
	int t_stack_cnt;
	d_batch batch;
	d_tex_conf tex_conf;
} d_gfx_ctx;

static d_gfx_ctx d_gfx;

void d_gfx_init() {

	// init gl
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glStencilFunc(GL_EQUAL, 1, 0xff);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// init default tex conf
	d_gfx.tex_conf = (d_tex_conf) {
		.filter = D_NEAREST,
		.wrap = D_REPEAT,
	};

	// init default shader
	d_gfx.default_shader = d_make_shader(NULL, NULL);
	d_gfx.cur_shader = &d_gfx.default_shader;

	// init default tex
	d_gfx.default_tex = d_make_tex((unsigned char[]){255, 255, 255, 255}, 1, 1);
	d_gfx.tex_slots[0] = &d_gfx.default_tex;

	// init default font
	d_gfx.default_font = d_make_font(d_parse_tex(unscii_png, unscii_png_len), 8, 8, D_ASCII_CHARS);
	d_gfx.cur_font = &d_gfx.default_font;

	// init default cam
	d_gfx.default_cam.view = mat4u();
	d_gfx.default_cam.proj = mat4_ortho(d_width(), d_height(), NEAR, FAR);
	d_gfx.cur_cam = &d_gfx.default_cam;

	// init batched renderer
	d_gfx.batch = d_make_batch();

	// init transform
	d_gfx.transform = mat4u();

	glViewport(0, 0, d_width(), d_height());
	d_gl_check_errors();

}

void d_gfx_frame_end() {
	d_batch_flush(&d_gfx.batch);
	d_gl_check_errors();
	d_gfx.transform = mat4u();
	d_gfx.default_cam.proj = mat4_ortho(d_width(), d_height(), NEAR, FAR);
	d_gfx.cur_cam = &d_gfx.default_cam;
	glViewport(0, 0, d_width(), d_height());
}

d_mesh d_make_mesh(
	const d_vertex *verts,
	int num_verts,
	const d_index *indices,
	int num_indices
) {

	// vertex buffer
	GLuint vbuf;

	glGenBuffers(1, &vbuf);
	glBindBuffer(GL_ARRAY_BUFFER, vbuf);
	glBufferData(GL_ARRAY_BUFFER, num_verts * sizeof(d_vertex), verts, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// index buffer
	GLuint ibuf;

	glGenBuffers(1, &ibuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices * sizeof(d_index), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return (d_mesh) {
		.vbuf = vbuf,
		.ibuf = ibuf,
		.count = num_indices,
	};

}

void d_free_mesh(d_mesh *m) {
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
		.vqueue = {0},
		.iqueue = {0},
		.num_verts = 0,
		.num_indices = 0,
	};

}

void d_batch_flush(d_batch *m) {

	if (m->num_indices == 0 || m->num_verts == 0) {
		return;
	}

	glBindBuffer(GL_ARRAY_BUFFER, m->vbuf);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m->num_verts * sizeof(d_vertex), &m->vqueue);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->ibuf);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m->num_indices * sizeof(d_index), &m->iqueue);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	d_push();
	d_gfx.transform = mat4u();
	d_draw(m->vbuf, m->ibuf, m->num_indices);
	d_pop();

	m->num_verts = 0;
	m->num_indices = 0;
	memset(m->vqueue, 0, sizeof(m->vqueue));
	memset(m->iqueue, 0, sizeof(m->iqueue));

}

void d_batch_push(
	d_batch *m,
	const d_vertex *verts,
	int num_verts,
	const d_index *indices,
	int num_indices
) {

	if (m->num_verts + num_verts >= BATCH_VERT_COUNT || m->num_indices + num_indices >= BATCH_INDEX_COUNT) {
		d_batch_flush(m);
	}

	memcpy(&m->vqueue[m->num_verts], verts, num_verts * sizeof(d_vertex));
	memcpy(&m->iqueue[m->num_indices], indices, num_indices * sizeof(d_index));

	for (int i = 0; i < num_indices; i++) {
		m->iqueue[m->num_indices + i] += m->num_verts;
	}

	m->num_verts += num_verts;
	m->num_indices += num_indices;

}

void d_free_batch(d_batch *m) {
	glDeleteBuffers(1, &m->vbuf);
	glDeleteBuffers(1, &m->ibuf);
	m->vbuf = 0;
	m->ibuf = 0;
}

d_img d_make_img(const unsigned char *pixels, int width, int height) {
	int size = sizeof(unsigned char) * width * height * 4;
	unsigned char *data = malloc(size);
	memcpy(data, pixels, size);
	return (d_img) {
		.data = data,
		.width = width,
		.height = height,
	};
}

d_img d_parse_img(const unsigned char *bytes, int size) {

	int w, h;
	// TODO: stand alone img doesn't need this
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load_from_memory((unsigned char*)bytes, size, &w, &h, NULL, 4);
	d_assert(data, "failed to parse image\n");

	return (d_img) {
		.data = data,
		.width = w,
		.height = h,
	};

}

d_img d_load_img(const char *path) {
	size_t size;
	unsigned char *bytes = d_read_bytes(path, &size);
	d_img img = d_parse_img(bytes, size);
	free(bytes);
	return img;
}

void d_img_write(d_img *img, const char *path) {
	stbi_flip_vertically_on_write(true);
	stbi_write_png(path, img->width, img->height, 4, img->data, img->width * 4);
}

void d_free_img(d_img *img) {
	free(img->data);
	img->data = NULL;
}

d_tex d_make_tex_ex(const unsigned char *data, int w, int h, d_tex_conf conf) {

	GLuint tex;

	glGenTextures(1, &tex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		w,
		h,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		data
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, conf.filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, conf.filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, conf.wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, conf.wrap);

	glBindTexture(GL_TEXTURE_2D, 0);

	return (d_tex) {
		.id = tex,
		.width = w,
		.height = h,
	};

}

d_tex d_make_tex(const unsigned char *data, int w, int h) {
	return d_make_tex_ex(data, w, h, d_gfx.tex_conf);
}

d_tex d_img_tex_ex(const d_img *img, d_tex_conf conf) {
	return d_make_tex_ex(img->data, img->width, img->height, conf);
}

d_tex d_img_tex(const d_img *img) {
	return d_img_tex_ex(img, d_gfx.tex_conf);
}

d_tex d_parse_tex_ex(const unsigned char *bytes, int size, d_tex_conf conf) {

	d_img img = d_parse_img(bytes, size);
	d_tex tex = d_img_tex_ex(&img, conf);
	d_free_img(&img);

	return tex;

}

d_tex d_parse_tex(const unsigned char *bytes, int size) {
	return d_parse_tex_ex(bytes, size, d_gfx.tex_conf);
}

d_tex d_load_tex_ex(const char *path, d_tex_conf conf) {

	d_img img = d_load_img(path);
	d_tex tex = d_img_tex_ex(&img, conf);
	d_free_img(&img);

	return tex;
}

d_tex d_load_tex(const char *path) {
	return d_load_tex_ex(path, d_gfx.tex_conf);
}

void d_free_tex(d_tex *t) {
	glDeleteTextures(1, &t->id);
	t->id = 0;
}

// static void parse_gltf_node(cgltf_node *node, d_model *model) {
	// ...
// }

// TODO
d_model d_parse_model(const unsigned char *bytes, int size) {

	cgltf_options options = {0};
	cgltf_data *data = NULL;
	cgltf_result res = cgltf_parse(&options, bytes, size, &data);

	d_assert(res == cgltf_result_success, "failed to parse gltf\n");
	d_assert(data->scene->nodes_count >= 1, "empty gltf\n");

	d_model model = {0};
	cgltf_node *node = data->scene->nodes[0];
	cgltf_mesh *mesh = node->mesh;

	for (int i = 0; i < mesh->primitives_count; i++) {
		cgltf_primitive *prim = &mesh->primitives[i];
// 		cgltf_accessor *indices = prim->indices;
		// TODO: why is the buffer view type invalid?
		for (int j = 0; j < prim->attributes_count; j++) {
// 			cgltf_attribute *attr = &prim->attributes[j];
// 			cgltf_accessor *data = attr->data;
		}
	}

	cgltf_free(data);

	return model;

}

d_model d_load_model(const char *path) {
	size_t size;
	unsigned char *bytes = d_read_bytes(path, &size);
	d_model model = d_parse_model(bytes, size);
	free(bytes);
	return model;
}

void d_free_model(d_model *model) {
	for (int i = 0; i < model->num_meshes; i++) {
		d_free_mesh(&model->meshes[i]);
	}
	for (int i = 0; i < model->num_children; i++) {
		d_free_model(&model->children[i]);
	}
	free(model->children);
	free(model->meshes);
}

d_font d_make_font(d_tex tex, int gw, int gh, const char *chars) {

	d_font f = {0};

	int cols = tex.width / gw;
	int rows = tex.height / gh;
	int count = cols * rows;

	f.qw = 1.0 / cols;
	f.qh = 1.0 / rows;
	f.tex = tex;

	d_assert(count == strlen(chars), "invalid font\n");

	for (int i = 0; i < count; i++) {
		f.map[(int)chars[i]] = (vec2) {
			.x = (i % cols) * f.qw,
			.y = (i / cols) * f.qh,
		};
	}

	return f;

}

void d_free_font(d_font *f) {
	d_free_tex(&f->tex);
}

d_shader d_make_shader(const char *vs_src, const char *fs_src) {

	if (vs_src == NULL) {
		vs_src = d_vert_default;
	}

	if (fs_src == NULL) {
		fs_src = d_frag_default;
	}

	const char *vs_code = strsub(d_vert_template, "##USER##", vs_src);
	const char *fs_code = strsub(d_frag_template, "##USER##", fs_src);

	GLchar info_log[512];
	GLint success = 0;

	// vertex shader
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vs, 1, &vs_code, 0);
	glCompileShader(vs);

	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE) {
		glGetShaderInfoLog(vs, 512, NULL, info_log);
		d_fail("%s", info_log);
	}

	// fragment shader
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fs, 1, &fs_code, 0);
	glCompileShader(fs);

	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE) {
		glGetShaderInfoLog(fs, 512, NULL, info_log);
		d_fail("%s", info_log);
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

	glGetProgramiv(program, GL_LINK_STATUS, &success);

	if (success == GL_FALSE) {
		glGetProgramInfoLog(program, 512, NULL, info_log);
		d_fail("%s", info_log);
	}

	free((void*)vs_code);
	free((void*)fs_code);

	return (d_shader) {
		.id = program,
	};

}

d_shader d_load_shader(const char *vs_path, const char *fs_path) {

	char *vs_src = NULL;
	char *fs_src = NULL;

	if (vs_path) {
		vs_src = d_read_text(vs_path);
	}

	if (fs_path) {
		fs_src = d_read_text(fs_path);
	}

	d_shader s = d_make_shader(vs_src, fs_src);

	if (vs_src) {
		free(vs_src);
	}

	if (fs_src) {
		free(fs_src);
	}

	return s;

}

void d_free_shader(d_shader *p) {
	glDeleteProgram(p->id);
	p->id = 0;
}

d_canvas d_make_canvas(int w, int h) {
	return d_make_canvas_ex(w, h, d_gfx.tex_conf);
}

// TODO: depth stencil texture with GLES
d_canvas d_make_canvas_ex(int w, int h, d_tex_conf conf) {

	unsigned char *buf = calloc(w * h * 4, sizeof(unsigned char));
	d_tex ctex = d_make_tex_ex(buf, w, h, conf);
	free(buf);

	GLuint dstex;
	glGenTextures(1, &dstex);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, dstex);

#ifndef GLES
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_DEPTH24_STENCIL8,
		w,
		h,
		0,
		GL_DEPTH_STENCIL,
		GL_UNSIGNED_INT_24_8,
		NULL
	);
#endif

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, conf.filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, conf.filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, conf.wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, conf.wrap);

	glBindTexture(GL_TEXTURE_2D, 0);

	GLuint fbuf;
	glGenFramebuffers(1, &fbuf);

	glBindFramebuffer(GL_FRAMEBUFFER, fbuf);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ctex.id, 0);
#ifndef GLES
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, dstex, 0);
#endif
	d_clear();

	d_assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "failed to create framebuffer\n");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return (d_canvas) {
		.fbuf = fbuf,
		.ctex = ctex,
		.dstex = dstex,
		.width = w,
		.height = h,
	};

}

#ifndef GLES
d_img d_canvas_capture(const d_canvas *c) {
	unsigned char *buf = calloc(c->width * c->height * 4, sizeof(unsigned char));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, c->ctex.id);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf);
	glBindTexture(GL_TEXTURE_2D, 0);
	d_img img = d_make_img(buf, c->width, c->height);
	free(buf);
	return img;
}
#endif

void d_free_canvas(d_canvas *c) {
	glDeleteFramebuffers(1, &c->fbuf);
	glDeleteTextures(1, &c->dstex);
	c->fbuf = 0;
	c->dstex = 0;
	d_free_tex(&c->ctex);
}

GLint d_uniform_loc(const char *name) {
	GLint loc = glGetUniformLocation(d_gfx.cur_shader->id, name);
	d_assert(loc != -1, "uniform not found: '%s'\n", name);
	return loc;
}

void d_send_f(const char *name, float v) {
	glUseProgram(d_gfx.cur_shader->id);
	glUniform1f(d_uniform_loc(name), v);
}

void d_send_vec2(const char *name, vec2 v) {
	glUseProgram(d_gfx.cur_shader->id);
	glUniform2f(d_uniform_loc(name), v.x, v.y);
}

void d_send_vec3(const char *name, vec3 v) {
	glUseProgram(d_gfx.cur_shader->id);
	glUniform3f(d_uniform_loc(name), v.x, v.y, v.z);
}

void d_send_color(const char *name, color c) {
	glUseProgram(d_gfx.cur_shader->id);
	glUniform4f(d_uniform_loc(name), c.r, c.g, c.b, c.a);
}

void d_send_mat4(const char *name, mat4 m) {
	glUseProgram(d_gfx.cur_shader->id);
	glUniformMatrix4fv(d_uniform_loc(name), 1, GL_FALSE, &m.m[0]);
}

void d_send_tex(const char *name, int idx, const d_tex *tex) {
	glUseProgram(d_gfx.cur_shader->id);
	if (tex) {
		glUniform1i(d_uniform_loc(name), idx + 1);
	}
	d_gfx.tex_slots[idx + 1] = tex;
}

void d_clear() {
	d_batch_flush(&d_gfx.batch);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void d_clear_color() {
	d_batch_flush(&d_gfx.batch);
	glClear(GL_COLOR_BUFFER_BIT);
}

void d_clear_depth() {
	d_batch_flush(&d_gfx.batch);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void d_clear_stencil() {
	d_batch_flush(&d_gfx.batch);
	glClear(GL_STENCIL_BUFFER_BIT);
}

void d_depth_write(bool b) {
	d_batch_flush(&d_gfx.batch);
	glDepthMask(b);
}

void d_depth_test(bool b) {
	d_batch_flush(&d_gfx.batch);
	if (b) {
		glEnable(GL_DEPTH_TEST);
	} else {
		glDisable(GL_DEPTH_TEST);
	}
}

void d_stencil_write(bool b) {
	d_batch_flush(&d_gfx.batch);
	if (b) {
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
		glStencilFunc(GL_NEVER, 1, 0xff);
	} else {
		glDisable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	}
}

void d_stencil_test(bool b) {
	d_batch_flush(&d_gfx.batch);
	if (b) {
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glStencilFunc(GL_EQUAL, 1, 0xff);
	} else {
		glDisable(GL_STENCIL_TEST);
	}
}

void d_blend_mode(d_blend b) {
	switch (b) {
		case D_BLEND_ALPHA:
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			break;
		case D_BLEND_ADD:
			glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
			break;
		case D_BLEND_REPLACE:
			glBlendFunc(GL_SRC_ALPHA, GL_ZERO);
			break;
		default:
			break;
	}
}

void d_push() {
	d_assert(d_gfx.t_stack_cnt < T_STACKS, "transform stack overflow\n");
	d_gfx.t_stack[d_gfx.t_stack_cnt] = d_gfx.transform;
	d_gfx.t_stack_cnt++;
}

void d_pop() {
	d_assert(d_gfx.t_stack_cnt > 0, "transform stack underflow\n");
	d_gfx.t_stack_cnt--;
	d_gfx.transform = d_gfx.t_stack[d_gfx.t_stack_cnt];
}

void d_move(vec3 p) {
	d_gfx.transform = mat4_mult(d_gfx.transform, mat4_translate(p));
}

void d_move_x(float x) {
	d_move(vec3f(x, 0.0, 0.0));
}

void d_move_y(float y) {
	d_move(vec3f(0.0, y, 0.0));
}

void d_move_z(float z) {
	d_move(vec3f(0.0, 0.0, z));
}

void d_move_xy(vec2 p) {
	d_move(vec3f(p.x, p.y, 0.0));
}

void d_scale(vec3 s) {
	d_gfx.transform = mat4_mult(d_gfx.transform, mat4_scale(s));
}

void d_scale_x(float x) {
	d_scale(vec3f(x, 1.0, 1.0));
}

void d_scale_y(float y) {
	d_scale(vec3f(1.0, y, 1.0));
}

void d_scale_z(float z) {
	d_scale(vec3f(1.0, 1.0, z));
}

void d_scale_xy(vec2 p) {
	d_scale(vec3f(p.x, p.y, 1.0));
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

void d_use_cam(const d_cam *cam) {
	if (cam) {
		d_gfx.cur_cam = cam;
	} else {
		d_gfx.cur_cam = &d_gfx.default_cam;
	}
}

void d_use_shader(const d_shader *shader) {
	d_batch_flush(&d_gfx.batch);
	if (shader) {
		d_gfx.cur_shader = shader;
	} else {
		d_gfx.cur_shader = &d_gfx.default_shader;
	}
}

void d_use_font(const d_font *font) {
	if (font) {
		d_gfx.cur_font = font;
	} else {
		d_gfx.cur_font = &d_gfx.default_font;
	}
}

void d_use_canvas(const d_canvas *c) {
	d_batch_flush(&d_gfx.batch);
	d_gfx.cur_canvas = c;
	if (c) {
		d_gfx.default_cam.proj = mat4_ortho(c->width, c->height, NEAR, FAR);
		glViewport(0, 0, c->width, c->height);
	} else {
		d_gfx.default_cam.proj = mat4_ortho(d_width(), d_height(), NEAR, FAR);
		glViewport(0, 0, d_width(), d_height());
	}
}

vec2 d_origin_pt(d_origin o) {
	switch (o) {
		case D_TOP_LEFT: return vec2f(-0.5, 0.5);
		case D_TOP: return vec2f(0, 0.5);
		case D_TOP_RIGHT: return vec2f(0.5, 0.5);
		case D_LEFT: return vec2f(-0.5, 0);
		case D_CENTER: return vec2f(0, 0);
		case D_RIGHT: return vec2f(0.5, 0);
		case D_BOT_LEFT: return vec2f(-0.5, -0.5);
		case D_BOT: return vec2f(0, -0.5);
		case D_BOT_RIGHT: return vec2f(0.5, -0.5);
	}
	return vec2f(0.0, 0.0);
}

vec2 d_coord(d_origin o) {
	vec2 p = d_origin_pt(o);
	return vec2f(p.x * d_width(), p.y * d_height());
}

mat4 d_transform() {
	return d_gfx.transform;
}

vec2 d_mouse_pos_t() {
	return mat4_mult_vec2(mat4_invert(d_transform()), d_mouse_pos());
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

	if (d_gfx.cur_canvas) {
		glBindFramebuffer(GL_FRAMEBUFFER, d_gfx.cur_canvas->fbuf);
	}

	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	for (int i = 0; d_gfx.tex_slots[i] != NULL; i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glUseProgram(0);

}

static void d_set_tex(const d_tex *t) {
	if (t) {
		if (t->id != d_gfx.tex_slots[0]->id) {
			d_batch_flush(&d_gfx.batch);
			d_gfx.tex_slots[0] = t;
		}
	} else {
		if (d_gfx.tex_slots[0]->id != d_gfx.default_tex.id) {
			d_batch_flush(&d_gfx.batch);
		}
		d_gfx.tex_slots[0] = &d_gfx.default_tex;
	}
}

void d_draw_raw(
	const d_vertex *verts,
	int num_verts,
	const d_index *indices,
	int num_indices,
	const d_tex *tex
) {

	d_vertex tverts[num_verts];
	memcpy(&tverts, verts, num_verts * sizeof(d_vertex));

	for (int i = 0; i < num_verts; i++) {
		tverts[i].pos = mat4_mult_vec3(d_gfx.transform, verts[i].pos);
	}

	d_set_tex(tex);
	d_batch_push(&d_gfx.batch, tverts, num_verts, indices, num_indices);

}

void d_draw_mesh(const d_mesh *mesh) {
	d_batch_flush(&d_gfx.batch);
	d_draw(mesh->vbuf, mesh->ibuf, mesh->count);
}

void d_draw_tex(const d_tex *t, quad q, color c) {

	d_push();
	d_scale(vec3f(t->width * q.w, t->height * q.h, 1.0));

	vec2 uv0 = vec2f(q.x, 1.0 - q.y - q.h);
	vec2 uv1 = vec2f(q.x, 1.0 - q.y);
	vec2 uv2 = vec2f(q.x + q.w, 1.0 - q.y);
	vec2 uv3 = vec2f(q.x + q.w, 1.0 - q.y - q.h);

	d_vertex verts[] = {
		{
			.pos = vec3f(-0.5, -0.5, 0.0),
			.normal = vec3f(0.0, 0.0, 1.0),
			.uv = uv0,
			.color = c
		},
		{
			.pos = vec3f(-0.5, 0.5, 0.0),
			.normal = vec3f(0.0, 0.0, 1.0),
			.uv = uv1,
			.color = c
		},
		{
			.pos = vec3f(0.5, 0.5, 0.0),
			.normal = vec3f(0.0, 0.0, 1.0),
			.uv = uv2,
			.color = c
		},
		{
			.pos = vec3f(0.5, -0.5, 0.0),
			.normal = vec3f(0.0, 0.0, 1.0),
			.uv = uv3,
			.color = c
		},
	};

	d_index indices[] = {
		0, 1, 2,
		0, 2, 3,
	};

	d_draw_raw(verts, 4, indices, 6, t);

	d_pop();

}

void d_draw_ftext(const d_ftext *ftext) {

	for (int i = 0; i < ftext->len; i++) {
		d_fchar ch = ftext->chars[i];
		d_push();
		d_move_xy(ch.pos);
		d_scale_xy(vec2f(ftext->scale, ftext->scale));
		d_draw_tex(ftext->tex, ch.quad, ch.color);
		d_pop();
	}

}

void d_draw_text(const char *text, float size, float wrap, d_origin orig, color c) {
	d_ftext ftext = d_fmt_text(text, size, wrap, orig, c);
	d_draw_ftext(&ftext);
}

void d_draw_canvas(const d_canvas *canvas, color c) {
	d_draw_tex(&canvas->ctex, quadu(), c);
}

void d_draw_rect(vec2 p1, vec2 p2, color c) {

	d_vertex verts[] = {
		{
			.pos = vec3f(p1.x, p1.y, 0.0),
			.normal = vec3f(0.0, 0.0, 1.0),
			.uv = vec2f(0.0, 0.0),
			.color = c
		},
		{
			.pos = vec3f(p1.x, p2.y, 0.0),
			.normal = vec3f(0.0, 0.0, 1.0),
			.uv = vec2f(0.0, 1.0),
			.color = c
		},
		{
			.pos = vec3f(p2.x, p2.y, 0.0),
			.normal = vec3f(0.0, 0.0, 1.0),
			.uv = vec2f(1.0, 1.0),
			.color = c
		},
		{
			.pos = vec3f(p2.x, p1.y, 0.0),
			.normal = vec3f(0.0, 0.0, 1.0),
			.uv = vec2f(1.0, 0.0),
			.color = c
		},
	};

	d_index indices[] = {
		0, 1, 2,
		0, 2, 3,
	};

	d_draw_raw(verts, 4, indices, 6, NULL);

}

void d_draw_line(vec2 p1, vec2 p2, float width, color c) {

	vec2 dpos1 = vec2_scale(vec2_unit(vec2_normal(vec2_sub(p2, p1))), width / 2.0);
	vec2 dpos2 = vec2_scale(vec2_unit(vec2_normal(vec2_sub(p1, p2))), width / 2.0);
	vec2 cp1 = vec2_sub(p1, dpos1);
	vec2 cp2 = vec2_add(p1, dpos1);
	vec2 cp3 = vec2_sub(p2, dpos2);
	vec2 cp4 = vec2_add(p2, dpos2);

	d_vertex verts[] = {
		{
			.pos = vec3f(cp1.x, cp1.y, 0.0),
			.normal = vec3f(0.0, 0.0, 1.0),
			.uv = vec2f(0.0, 0.0),
			.color = c
		},
		{
			.pos = vec3f(cp2.x, cp2.y, 0.0),
			.normal = vec3f(0.0, 0.0, 1.0),
			.uv = vec2f(0.0, 0.0),
			.color = c
		},
		{
			.pos = vec3f(cp3.x, cp3.y, 0.0),
			.normal = vec3f(0.0, 0.0, 1.0),
			.uv = vec2f(0.0, 0.0),
			.color = c
		},
		{
			.pos = vec3f(cp4.x, cp4.y, 0.0),
			.normal = vec3f(0.0, 0.0, 1.0),
			.uv = vec2f(0.0, 0.0),
			.color = c
		},
	};

	d_index indices[] = {
		0, 1, 2,
		0, 2, 3,
	};

	d_draw_raw(verts, 4, indices, 6, NULL);

}

void d_draw_lrect(vec2 p1, vec2 p3, float w, color c) {
	vec2 p2 = vec2f(p1.x, p3.y);
	vec2 p4 = vec2f(p3.x, p1.y);
	d_draw_line(p1, p2, w, c);
	d_draw_line(p2, p3, w, c);
	d_draw_line(p3, p4, w, c);
	d_draw_line(p4, p1, w, c);
}

// TODO
void d_draw_circle(vec2 p, float r, color c) {
	// ...
}

// TODO: support \n
d_ftext d_fmt_text(const char *text, float size, float wrap, d_origin origin, color c) {

	d_ftext ftext = {0};

	const d_tex *tex = &d_gfx.cur_font->tex;
	int len = strlen(text);
	float qw = d_gfx.cur_font->qw;
	float qh = d_gfx.cur_font->qh;
	float scale = size / (qh * tex->height);
	float gw = qw * tex->width * scale;
	float gh = size;
	vec2 offset = d_origin_pt(origin);

	float line_width = 0.0;
	float text_width = 0.0;
	float cur_y = 0.0;
	int last_space = -1;
	int last_i = 0;
	int actual_len = 0;

	for (int i = 0; i < len; i++) {

		char ch = text[i];

		if (!isprint(ch)) {
			continue;
		}

		if (ch == ' ') {
			last_space = i;
		}

		line_width += gw;

		bool overflow = wrap != 0.0 && (line_width + gw) > wrap;
		bool last = i == len - 1;

		if (overflow || last) {

			int to = last ? i : (last_space >= 0 ? last_space : i);
			float rlw = (to - last_i + 1) * gw;
			float ox = 0.5 * gw - rlw * (offset.x + 0.5);
			float x = 0.0;

			for (int j = last_i; j <= to; j++) {

				char ch = text[j];

				if (!isprint(ch)) {
					continue;
				}

				vec2 qpos = d_gfx.cur_font->map[(int)ch];
				quad q = quadf(qpos.x, qpos.y, qw, qh);

				ftext.chars[actual_len++] = (d_fchar) {
					.pos = vec2f(x + ox, cur_y),
					.color = c,
					.quad = q,
				};

				x += gw;

			}

			if (rlw > text_width) {
				text_width = rlw;
			}

			line_width = 0.0;
			last_i = to + 1;
			i = to;
			cur_y -= gh;
			last_space = -1;

		}

	}

	int text_height = -cur_y;

	ftext.len = actual_len;
	ftext.width = wrap != 0.0 ? wrap : text_width;
	ftext.height = text_height;
	ftext.tex = tex;
	ftext.cw = gw;
	ftext.ch = gh;
	ftext.scale = scale;
	ftext.origin = origin;

	float oy = -0.5 * gh - (offset.y - 0.5) * text_height;

	for (int i = 0; i < ftext.len; i++) {
		ftext.chars[i].pos.y += oy;
	}

	return ftext;

}

vec2 d_ftext_cpos(const d_ftext *ftext, int cursor) {

	if (ftext->len == 0) {
		return vec2f(0.0, 0.0);
	}

	if (cursor == ftext->len) {
		return vec2_add(d_ftext_cpos(ftext, ftext->len - 1), vec2f(ftext->cw, 0.0));
	}

	return vec2_add(ftext->chars[cursor].pos, vec2_mult(d_origin_pt(ftext->origin), vec2f(ftext->cw, ftext->ch)));

}

const char *d_gl_version() {
	return (const char*)glGetString(GL_VERSION);
}

const char *d_glsl_version() {
	return (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
}

const char *d_gl_renderer() {
	return (const char*)glGetString(GL_RENDERER);
}

const char *d_gl_vendor() {
	return (const char*)glGetString(GL_VENDOR);
}

void d_gl_check_errors() {

	GLuint err = glGetError();

	while (err != GL_NO_ERROR) {

		switch (err) {
			case GL_INVALID_ENUM:
				d_fail("gl invalid enum\n");
				break;
			case GL_INVALID_VALUE:
				d_fail("gl invalid value\n");
				break;
			case GL_INVALID_OPERATION:
				d_fail("gl invalid operation\n");
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				d_fail("gl invalid framebuffer operation\n");
				break;
			case GL_OUT_OF_MEMORY:
				d_fail("gl out of memory\n");
				break;
#ifndef GLES
			case GL_STACK_UNDERFLOW:
				d_fail("gl stack underflow\n");
				break;
			case GL_STACK_OVERFLOW:
				d_fail("gl stack overflow\n");
				break;
#endif
		}

		err = glGetError();

	}

}

static struct json_object_element_s *json_get(struct json_object_s *obj, const char *name) {
	struct json_object_element_s *cur = obj->start;
	while (cur) {
		if (strcmp(cur->name->string, name) == 0) {
			return cur;
		}
		cur = cur->next;
	}
	d_fail("expected '%s' key\n", name);
	return NULL;
}

static struct json_object_s *json_get_obj(struct json_object_s *obj, const char *name) {
	struct json_object_element_s *el = json_get(obj, name);
	struct json_object_s *child = json_value_as_object(el->value);
	d_assert(child, "expected '%s' to be an object", name);
	return child;
}

static float json_get_num(struct json_object_s *obj, const char *name) {
	struct json_object_element_s *el = json_get(obj, name);
	struct json_number_s *child = json_value_as_number(el->value);
	d_assert(child, "expected '%s' to be a number", name);
	return atof(child->number);
}

static struct json_array_s *json_get_arr(struct json_object_s *obj, const char *name) {
	struct json_object_element_s *el = json_get(obj, name);
	struct json_array_s *child = json_value_as_array(el->value);
	d_assert(child, "expected '%s' to be an array", name);
	return child;
}

static const char *json_get_str(struct json_object_s *obj, const char *name) {
	struct json_object_element_s *el = json_get(obj, name);
	struct json_string_s *child = json_value_as_string(el->value);
	d_assert(child, "expected '%s' to be a string", name);
	return child->string;
}

// static bool json_get_bool(struct json_object_s *obj, const char *name) {
// 	struct json_object_element_s *el = json_get(obj, name);
// 	if (json_value_is_true(el->value)) {
// 		return true;
// 	} else if (json_value_is_false(el->value)) {
// 		return false;
// 	}
// 	d_fail("expected '%s' to be a boolean", name);
// 	return false;
// }

d_sprite_data d_parse_ase(const char *json) {

	struct json_value_s *root = json_parse(json, strlen(json));
	struct json_object_s *obj = json_value_as_object(root);
	d_assert(obj, "failed to parse json\n");
	struct json_array_s *frames = json_get_arr(obj, "frames");
	struct json_object_s *meta = json_get_obj(obj, "meta");

	struct json_object_s *size = json_get_obj(meta, "size");
	float sw = json_get_num(size, "w");
	float sh = json_get_num(size, "h");

	quad *jframes = calloc(frames->length, sizeof(quad));

	struct json_array_element_s *cur_frame = frames->start;
	int cur_frame_idx = 0;

	while (cur_frame) {

		struct json_object_s *data = json_get_obj(json_value_as_object(cur_frame->value), "frame");
		float fx = json_get_num(data, "x");
		float fy = json_get_num(data, "y");
		float fw = json_get_num(data, "w");
		float fh = json_get_num(data, "h");

		jframes[cur_frame_idx] = quadf(fx / sw, fy / sh, fw / sw, fh / sh);

		cur_frame = cur_frame->next;
		cur_frame_idx++;

	}

	struct json_array_s *tags = json_get_arr(meta, "frameTags");
	d_sprite_anim *anims = calloc(tags->length, sizeof(d_sprite_anim));

	struct json_array_element_s *cur_tag = tags->start;
	int cur_tag_idx = 0;

	while (cur_tag) {

		struct json_object_s *data = json_value_as_object(cur_tag->value);

		const char *name = json_get_str(data, "name");
		const char *dir = json_get_str(data, "direction");
		int from = (int)json_get_num(data, "from");
		int to = (int)json_get_num(data, "to");

		if (strcmp(dir, "reverse") == 0) {
			int tmp = from;
			from = to;
			to = tmp;
		}

		anims[cur_tag_idx] = (d_sprite_anim) {
			.name = name,
			.from = from,
			.to = to,
		};

		cur_tag = cur_tag->next;
		cur_tag_idx++;

	}

	free(root);

	return (d_sprite_data) {
		.frames = jframes,
		.num_frames = frames->length,
		.anims = anims,
		.num_anims = tags->length,
		.w = sw,
		.h = sh,
	};

}

d_sprite_data d_load_ase(const char *path) {
	char *content = d_read_text(path);
	d_sprite_data data = d_parse_ase(content);
	free(content);
	return data;
}

void d_free_sprite_data(d_sprite_data *data) {
	free(data->frames);
	free(data->anims);
	data->frames = NULL;
	data->anims = NULL;
}

