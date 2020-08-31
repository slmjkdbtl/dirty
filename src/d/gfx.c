// wengwengweng

#include <OpenGL/gl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "gfx.h"
#include "app.h"

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
"{{user}}"
"void main() {"
	"v_pos = a_pos;"
	"v_uv = a_uv;"
	"v_color = a_color;"
	"v_normal = normalize(a_normal);"
// 	"gl_Position = vert();"
	"gl_Position = vec4(a_pos, 1.0);"
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
"{{user}}"
"void main() {"
// 	"gl_FragColor = frag();"
	"gl_FragColor = v_color * u_color;"
	"if (gl_FragColor.a == 0.0) {"
		"discard;"
	"}"
"}"
;

d_mesh d_make_mesh(d_vertex* verts, size_t verts_size, unsigned int* indices, size_t indices_size) {

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
	};

}

d_tex2d d_make_tex(unsigned char* data, int w, int h) {

	GLuint tex;

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

	glTexParameter(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER);

	glBindTexture(GL_TEXTURE_2D, 0);

	return (d_tex2d) {
		.id = tex,
	};

}

char* strsub(const char* str, const char* old, const char* new) {

	char* res;
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

	res = (char*)malloc(i + cnt * (new_len - old_len) + 1);
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

	// TODO: possible leak
	vs_src = strsub(vert_template, "{{user}}", vs_src);
	fs_src = strsub(frag_template, "{{user}}", fs_src);

	int info_len;
	char info_log[512];

	// vertex shader
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vs, 1, &vs_src, 0);
	glCompileShader(vs);

	glGetShaderInfoLog(vs, 512, &info_len, info_log);
	fprintf(stderr, "%s", info_log);

	// fragment shader
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fs, 1, &fs_src, 0);
	glCompileShader(fs);

	glGetShaderInfoLog(vs, 512, &info_len, info_log);
	fprintf(stderr, "%s", info_log);

	// program
	GLuint program = glCreateProgram();

	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glBindAttribLocation(program, 0, "a_pos");
	glBindAttribLocation(program, 1, "a_normal");
	glBindAttribLocation(program, 2, "a_uv");
	glBindAttribLocation(program, 3, "a_color");

	glLinkProgram(program);

	glGetProgramInfoLog(program, 512, &info_len, info_log);
	fprintf(stderr, "%s", info_log);

	return (d_program) {
		.id = program,
	};

}

void d_send_f(const char* name, float v) {
	glUniform1f(glGetUniformLocation(d.cur_prog->id, name), v);
}

void d_send_vec2(const char* name, vec2 v) {
	glUniform2f(glGetUniformLocation(d.cur_prog->id, name), v.x, v.y);
}

void d_send_vec3(const char* name, vec3 v) {
	glUniform3f(glGetUniformLocation(d.cur_prog->id, name), v.x, v.y, v.z);
}

void d_send_color(const char* name, color c) {
	glUniform4f(glGetUniformLocation(d.cur_prog->id, name), c.r, c.g, c.b, c.a);
}

void d_draw(d_mesh* mesh, d_program* program) {

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibuf);
	glUseProgram(program->id);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 48, (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 48, (void*)12);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 48, (void*)24);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 48, (void*)32);
	glEnableVertexAttribArray(3);

	d_send_color("u_color", (color) { 1.0, 1.0, 1.0, 1.0, });

	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);

}

