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

#include "utils.h"
// http://pelulamu.net/unscii/
#include "res/unscii.png.h"

#define T_STACKS 16

static const char *vs_src =
#ifndef GLES
"#version 120\n"
#endif
"attribute vec2 a_pos;"
"attribute vec2 a_uv;"
"varying vec2 v_uv;"
"void main() {"
	"v_uv = a_uv;"
	"gl_Position = vec4(a_pos, 0.0, 1.0);"
"}"
;

static const char *fs_src =
#ifndef GLES
"#version 120\n"
#else
"precision mediump float;"
#endif
"varying vec2 v_uv;"
"uniform sampler2D u_tex;"
"void main() {"
	"gl_FragColor = texture2D(u_tex, v_uv);"
	"if (gl_FragColor.a == 0.0) {"
		"discard;"
	"}"
"}"
;

typedef struct {
	GLuint gl_prog;
	GLuint gl_vbuf;
	GLuint gl_ibuf;
	GLuint gl_tex;
	d_img canvas;
} d_gfx_ctx;

static d_gfx_ctx d_gfx;

void d_gfx_init(d_desc *desc) {

	d_gfx.canvas = d_make_img(desc->width, desc->height);

	// program
	GLchar info_log[512];
	GLint success = 0;

	// vertex shader
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vs, 1, &vs_src, 0);
	glCompileShader(vs);

	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE) {
		glGetShaderInfoLog(vs, 512, NULL, info_log);
		d_fail("%s", info_log);
	}

	// fragment shader
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fs, 1, &fs_src, 0);
	glCompileShader(fs);

	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE) {
		glGetShaderInfoLog(fs, 512, NULL, info_log);
		d_fail("%s", info_log);
	}

	// program
	d_gfx.gl_prog = glCreateProgram();

	glAttachShader(d_gfx.gl_prog, vs);
	glAttachShader(d_gfx.gl_prog, fs);

	glBindAttribLocation(d_gfx.gl_prog, 0, "a_pos");
	glBindAttribLocation(d_gfx.gl_prog, 1, "a_uv");

	glLinkProgram(d_gfx.gl_prog);

	glDetachShader(d_gfx.gl_prog, vs);
	glDetachShader(d_gfx.gl_prog, fs);
	glDeleteShader(vs);
	glDeleteShader(fs);

	glGetProgramiv(d_gfx.gl_prog, GL_LINK_STATUS, &success);

	if (success == GL_FALSE) {
		glGetProgramInfoLog(d_gfx.gl_prog, 512, NULL, info_log);
		d_fail("%s", info_log);
	}

	float verts[] = {
		-1.0,  1.0, 0.0, 0.0,
		 1.0,  1.0, 1.0, 0.0,
		 1.0, -1.0, 1.0, 1.0,
		-1.0, -1.0, 0.0, 1.0,
	};

	unsigned int indices[] = {
		0, 1, 2,
		0, 2, 3,
	};

	// vbuf
	glGenBuffers(1, &d_gfx.gl_vbuf);
	glBindBuffer(GL_ARRAY_BUFFER, d_gfx.gl_vbuf);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// ibuf
	glGenBuffers(1, &d_gfx.gl_ibuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, d_gfx.gl_ibuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// tex
	glGenTextures(1, &d_gfx.gl_tex);
	glBindTexture(GL_TEXTURE_2D, d_gfx.gl_tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		d_gfx.canvas.width,
		d_gfx.canvas.height,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		d_gfx.canvas.pixels
	);

	glBindTexture(GL_TEXTURE_2D, 0);

	// init gl
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glViewport(0, 0, d_gfx.canvas.width * desc->scale, d_gfx.canvas.height * desc->scale);

}

void d_gfx_frame_end() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, d_gfx.gl_vbuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, d_gfx.gl_ibuf);
	glUseProgram(d_gfx.gl_prog);
	glBindTexture(GL_TEXTURE_2D, d_gfx.gl_tex);

	glTexSubImage2D(
		GL_TEXTURE_2D,
		0,
		0,
		0,
		d_gfx.canvas.width,
		d_gfx.canvas.height,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		d_gfx.canvas.pixels
	);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 16, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 16, (void*)8);
	glEnableVertexAttribArray(1);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);
	glBindTexture(GL_TEXTURE_2D, 0);

}

d_img d_make_img(int w, int h) {
	return (d_img) {
		.width = w,
		.height = h,
		.pixels = calloc(w * h, sizeof(color)),
	};
}

d_img d_parse_img(const unsigned char *bytes, int size) {

	int w, h;
	unsigned char *pixels = stbi_load_from_memory((unsigned char*)bytes, size, &w, &h, NULL, 4);
	d_assert(pixels, "failed to parse image\n");

	return (d_img) {
		.width = w,
		.height = h,
		.pixels = (color*)pixels,
	};

}

d_img d_load_img(const char *path) {
	size_t size;
	unsigned char *bytes = d_read_bytes(path, &size);
	d_img img = d_parse_img(bytes, size);
	free(bytes);
	return img;
}

void d_free_img(d_img *img) {
	free(img->pixels);
	img->pixels = NULL;
	img->width = 0;
	img->height = 0;
}

static int get_index(vec2 p) {
	return (int)(p.y * d_gfx.canvas.width + p.x);
}

void d_clear() {
	memset(d_gfx.canvas.pixels, 0, sizeof(color) * d_gfx.canvas.width * d_gfx.canvas.height);
}

void d_set_pixel(vec2 p, color c) {
	if (p.x < 0 || p.x >= d_gfx.canvas.width || p.y < 0 || p.y >= d_gfx.canvas.height) {
		return;
	}
	int i = get_index(p);
	d_gfx.canvas.pixels[i] = c;
}

color d_get_pixel(vec2 p) {
	if (p.x < 0 || p.x >= d_gfx.canvas.width || p.y < 0 || p.y >= d_gfx.canvas.height) {
		return colori(0, 0, 0, 0);
	}
	int i = get_index(p);
	return d_gfx.canvas.pixels[i];
}

void d_draw_img(const d_img *img, vec2 pos) {
	for (int x = 0; x < img->width; x++) {
		for (int y = 0; y < img->height; y++) {
			d_set_pixel(vec2f(x + pos.x, y + pos.y), img->pixels[y * img->width + x]);
		}
	}
}

void d_draw_tri(vec2 p1, vec2 p2, vec2 p3, color c) {
	// TODO
}

void d_draw_rect(vec2 p1, vec2 p2, color c) {

	int x1 = p1.x < p2.x ? p1.x : p2.x;
	int x2 = p1.x > p2.x ? p1.x : p2.x;
	int y1 = p1.y < p2.y ? p1.y : p2.y;
	int y2 = p1.y > p2.y ? p1.y : p2.y;

	for (int i = x1; i < x2; i++) {
		for (int j = y1; j < y2; j++) {
			d_set_pixel(vec2f(i, j), c);
		}
	}

}

void d_draw_line(vec2 p1, vec2 p2, color c) {

	int dx = p2.x - p1.x;
	int dy = p2.y - p1.y;
	int adx = abs(dx);
	int ady = abs(dy);
	int eps = 0;
	int sx = dx > 0 ? 1 : -1;
	int sy = dy > 0 ? 1 : -1;

	if (adx > ady) {
		for(int x = p1.x, y = p1.y; sx < 0 ? x >= p2.x : x <= p2.x; x += sx) {
			d_set_pixel(vec2f(x, y), c);
			eps += ady;
			if ((eps << 1) >= adx) {
				y += sy;
				eps -= adx;
			}
		}
	} else {
		for(int x = p1.x, y = p1.y; sy < 0 ? y >= p2.y : y <= p2.y; y += sy) {
			d_set_pixel(vec2f(x, y), c);
			eps += adx;
			if ((eps << 1) >= ady) {
				x += sx;
				eps -= ady;
			}
		}
	}

}

