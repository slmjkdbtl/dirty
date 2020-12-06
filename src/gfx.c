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

#include "res/unscii.png.h"

void d_present(color *pixels);

#define MAX_TSTACK 16

typedef struct {
	d_img def_canvas;
	d_img *cur_canvas;
	d_blend blend;
	d_font def_font;
	d_font *cur_font;
} d_gfx_ctx;

static d_gfx_ctx d_gfx;

void d_gfx_init(d_desc *desc) {
	d_gfx.def_canvas = d_make_img(desc->width, desc->height);
	d_gfx.cur_canvas = &d_gfx.def_canvas;
	d_gfx.def_font = d_make_font(d_parse_img(unscii_png, unscii_png_len), 8, 8, D_ASCII_CHARS);
	d_gfx.cur_font = &d_gfx.def_font;
	d_gfx.blend = D_ALPHA;
}

void d_gfx_frame_end() {
	d_present(d_gfx.cur_canvas->pixels);
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

d_imgs d_img_slice(const d_img *img, int w, int h) {
	int cols = img->width / w;
	int rows = img->height / h;
	int num_imgs = cols * rows;
	d_img *imgs = malloc(sizeof(d_img) * num_imgs);
	for (int i = 0; i < num_imgs; i++) {
		int ox = i % cols * w;
		int oy = i / cols * h;
		imgs[i] = d_make_img(w, h);
		for (int x = 0; x < w; x++) {
			for (int y = 0; y < h; y++) {
				d_img_set(&imgs[i], x, y, d_img_get(img, x + ox, y + oy));
			}
		}
	}
	return (d_imgs) {
		.imgs = imgs,
		.num_imgs = num_imgs,
	};
}

void d_img_shade(d_img *img, color (*shade)(color, int, int, int, int)) {
	for (int i = 0; i < img->width * img->height; i++) {
		int x = i % img->width;
		int y = i / img->height;
		img->pixels[i] = shade(img->pixels[i], x, y, img->width, img->height);
	}
}

void d_img_set(d_img *img, int x, int y, color c) {
	if (x < 0 || x >= img->width || y < 0 || y >= img->height) {
		return;
	}
	int i = (int)(y * img->width + x);
	img->pixels[i] = c;
}

color d_img_get(const d_img *img, int x, int y) {
	if (x < 0 || x >= img->width || y < 0 || y >= img->height) {
		return colori(0, 0, 0, 0);
	}
	int i = (int)(y * img->width + x);
	return img->pixels[i];
}

void d_img_save(const d_img *img, const char *path) {
	stbi_write_png(path, img->width, img->height, 4, (unsigned char *)img->pixels, img->width * 4);
}

void d_free_img(d_img *img) {
	free(img->pixels);
	img->pixels = NULL;
	img->width = 0;
	img->height = 0;
}

void d_free_imgs(d_imgs *imgs) {
	for (int i = 0; i < imgs->num_imgs; i++) {
		d_free_img(&imgs->imgs[i]);
	}
	free(imgs->imgs);
	imgs->imgs = NULL;
	imgs->num_imgs = 0;
}

d_font d_make_font(d_img img, int gw, int gh, const char *chars) {

	d_font f = {0};
	f.width = gw;
	f.height = gh;
	f.imgs = d_img_slice(&img, gw, gh);

	int num_chars = strlen(chars);

	for (int i = 0; i < num_chars; i++) {
		f.map[(int)chars[i]] = i;
	}

	d_free_img(&img);

	return f;

}

void d_free_font(d_font *f) {
	d_free_imgs(&f->imgs);
}

void d_free_mesh(d_mesh *m) {
	free(m->verts);
	m->num_verts = 0;
	free(m->indices);
	m->num_indices = 0;
}

static void d_parse_model_node(cgltf_node *node, d_model_node *model) {

	model->psr.pos = vec3f(node->translation[0], node->translation[1], node->translation[2]);
	model->psr.scale = vec3f(node->scale[0], node->scale[1], node->scale[2]);
	model->psr.rot = quatf(node->rotation[0], node->rotation[1], node->rotation[2], node->rotation[3]);

	int num_children = node->children_count;

	model->num_children = num_children;
	model->children = malloc(sizeof(d_model_node) * num_children);

	for (int i = 0; i < num_children; i++) {
		d_parse_model_node(node->children[i], &model->children[i]);
	}

	int num_meshes = node->mesh->primitives_count;
	model->num_meshes = num_meshes;
	model->meshes = malloc(sizeof(d_mesh) * num_meshes);

	for (int i = 0; i < num_meshes; i++) {

		cgltf_primitive *prim = &node->mesh->primitives[i];
		int num_verts = prim->attributes[0].data->count;
		d_vertex *verts = calloc(num_verts, sizeof(d_vertex));

		for (int j = 0; j < num_verts; j++) {
			verts[j].color = coloru();
		}

		for (int j = 0; j < prim->attributes_count; j++) {

			cgltf_attribute *attr = &prim->attributes[j];
			cgltf_accessor *acc = attr->data;
			float color_tmp[4];

			d_assert(acc->count == num_verts, "bad gltf\n");

			switch (attr->type) {
				case cgltf_attribute_type_position:
					d_assert(acc->type == cgltf_type_vec3, "gltf pos must be vec3\n");
					for(int k = 0; k < acc->count; k++) {
						cgltf_accessor_read_float(acc, k, (float*)&verts[k].pos, 3);
					}
					break;
				case cgltf_attribute_type_normal:
					d_assert(acc->type == cgltf_type_vec3, "gltf normal must be vec3\n");
					for(int k = 0; k < acc->count; k++) {
						cgltf_accessor_read_float(acc, k, (float*)&verts[k].normal, 3);
					}
					break;
				case cgltf_attribute_type_texcoord:
					d_assert(acc->type == cgltf_type_vec2, "gltf texcoord must be vec2\n");
					for(int k = 0; k < acc->count; k++) {
						cgltf_accessor_read_float(acc, k, (float*)&verts[k].uv, 2);
					}
					break;
				case cgltf_attribute_type_color:
					d_assert(acc->type == cgltf_type_vec4, "gltf color must be vec4\n");
					for(int k = 0; k < acc->count; k++) {
						cgltf_accessor_read_float(acc, k, color_tmp, 4);
						verts[k].color = colorf(color_tmp[0], color_tmp[1], color_tmp[2], color_tmp[3]);
					}
					break;
				default:
					break;
			}

		}

		int num_indices = prim->indices->count;
		d_index *indices = calloc(num_indices, sizeof(d_index));

		for (int j = 0; j < prim->indices->count; j++) {
			cgltf_accessor_read_uint(prim->indices, j, &indices[j], 1);
		}

		model->meshes[i] = (d_mesh) {
			.verts = verts,
			.num_verts = num_verts,
			.indices = indices,
			.num_indices = num_indices,
		};

	}

}
d_model d_parse_model(const unsigned char *bytes, int size) {

	cgltf_options options = {0};
	cgltf_data *doc = NULL;
	cgltf_result res = cgltf_parse(&options, bytes, size, &doc);
	cgltf_load_buffers(&options, doc, NULL);

	d_assert(res == cgltf_result_success, "failed to parse gltf\n");
	d_assert(doc->scene->nodes_count >= 1, "empty gltf\n");

	d_model model = {0};

	int num_textures = doc->textures_count;
	model.num_images = num_textures;
	model.images = malloc(sizeof(d_img) * num_textures);

	for (int i = 0; i < num_textures; i++) {
		cgltf_image *img = doc->textures[i].image;
		cgltf_buffer_view *view = img->buffer_view;
		unsigned char *data = view->buffer->data;
		model.images[i] = d_parse_img(data + view->offset, view->size);
	}

	int num_nodes = doc->scene->nodes_count;
	model.num_nodes = num_nodes;
	model.nodes = malloc(sizeof(d_model_node) * num_nodes);

	for (int i = 0; i < num_nodes; i++) {
		d_parse_model_node(doc->scene->nodes[i], &model.nodes[i]);
	}

	cgltf_free(doc);

	return model;

}

d_model d_load_model(const char *path) {
	size_t size;
	unsigned char *bytes = d_read_bytes(path, &size);
	d_model data = d_parse_model(bytes, size);
	free(bytes);
	return data;
}

static void d_model_node_bbox(const d_model_node *node, box *bbox, mat4 tp) {

	mat4 t = mat4_mult(tp, d_psr_mat4(node->psr));

	for (int i = 0; i < node->num_meshes; i++) {
		d_mesh *mesh = &node->meshes[i];
		for (int j = 0; j < mesh->num_verts; j++) {
			vec3 p = mat4_mult_vec3(t, mesh->verts[j].pos);
			bbox->p1 = vec3_min(bbox->p1, p);
			bbox->p2 = vec3_max(bbox->p2, p);
		}
	}

	for (int i = 0; i < node->num_children; i++) {
		d_model_node_bbox(&node->children[i], bbox, t);
	}

}

box d_model_bbox(const d_model *data) {

	box bbox = (box) {
		.p1 = vec3f(0.0, 0.0, 0.0),
		.p2 = vec3f(0.0, 0.0, 0.0),
	};

	for (int i = 0; i < data->num_nodes; i++) {
		d_model_node_bbox(&data->nodes[i], &bbox, mat4u());
	}

	return bbox;

}

static void d_free_model_node(d_model_node *node) {
	for (int i = 0; i < node->num_meshes; i++) {
		d_free_mesh(&node->meshes[i]);
	}
	for (int i = 0; i < node->num_children; i++) {
		d_free_model_node(&node->children[i]);
	}
	free(node->children);
	free(node->meshes);
}

void d_free_model(d_model *model) {
	for (int i = 0; i < model->num_nodes; i++) {
		d_free_model_node(&model->nodes[i]);
	}
	for (int i = 0; i < model->num_images; i++) {
		d_free_img(&model->images[i]);
	}
	free(model->nodes);
	free(model->images);
}

mat4 d_psr_mat4(d_psr psr) {
	return mat4_mult(mat4_mult(mat4_translate(psr.pos), mat4_scale(psr.scale)), mat4_rot_quat(psr.rot));
}

void d_clear() {
	memset(d_gfx.cur_canvas->pixels, 0, sizeof(color) * d_gfx.cur_canvas->width * d_gfx.cur_canvas->height);
}

void d_put(vec2 p, color c) {
	switch (d_gfx.blend) {
		case D_ALPHA: {
			if (c.a == 255) {
				d_img_set(d_gfx.cur_canvas, p.x, p.y, c);
			} else if (c.a != 0) {
				color rc = d_img_get(d_gfx.cur_canvas, p.x, p.y);
				d_img_set(d_gfx.cur_canvas, p.x, p.y, (color) {
					.r = (rc.r * (255 - c.a) + c.r * c.a) / 255,
					.g = (rc.g * (255 - c.a) + c.g * c.a) / 255,
					.b = (rc.b * (255 - c.a) + c.b * c.a) / 255,
					.a = (rc.a * (255 - c.a) + c.a * c.a) / 255,
				});
			}
			break;
		}
		case D_REPLACE:
			d_img_set(d_gfx.cur_canvas, p.x, p.y, c);
			break;
		case D_ADD:
			if (c.a != 0) {
				color rc = d_img_get(d_gfx.cur_canvas, p.x, p.y);
				d_img_set(d_gfx.cur_canvas, p.x, p.y, (color) {
					.r = (rc.r * rc.a + c.r * c.a) / 255,
					.g = (rc.g * rc.a + c.g * c.a) / 255,
					.b = (rc.b * rc.a + c.b * c.a) / 255,
					.a = (rc.a * rc.a + c.a * c.a) / 255,
				});
			}
			break;
	}
}

color d_peek(vec2 p) {
	return d_img_get(d_gfx.cur_canvas, p.x, p.y);
}

void d_draw_img(const d_img *img, vec2 pos) {
	for (int x = 0; x < img->width; x++) {
		for (int y = 0; y < img->height; y++) {
			d_put(vec2f(x + pos.x, y + pos.y), img->pixels[y * img->width + x]);
		}
	}
}

void d_draw_tri(vec2 p1, vec2 p2, vec2 p3, color c) {

	if (c.a == 0) {
		return;
	}

	// TODO

}

void d_draw_rect(vec2 p1, vec2 p2, color c) {

	if (c.a == 0) {
		return;
	}

	int x1 = p1.x < p2.x ? p1.x : p2.x;
	int x2 = p1.x > p2.x ? p1.x : p2.x;
	int y1 = p1.y < p2.y ? p1.y : p2.y;
	int y2 = p1.y > p2.y ? p1.y : p2.y;

	for (int i = x1; i < x2; i++) {
		for (int j = y1; j < y2; j++) {
			d_put(vec2f(i, j), c);
		}
	}

}

void d_draw_circle(vec2 center, float r, color c) {

	if (c.a == 0) {
		return;
	}

	for (int i = center.x - r; i <= center.x + r; i++) {
		for (int j = center.y - r; j <= center.y + r; j++) {
			vec2 p = vec2f(i, j);
			float d = vec2_dist(p, center);
			if (d <= r) {
				d_put(p, c);
			}
		}
	}

}

void d_draw_text(const char *text, vec2 pos) {

	int num_chars = strlen(text);
	d_font *font = d_gfx.cur_font;
	int ox = 0;

	for (int i = 0; i < num_chars; i++) {
		int ii = font->map[(int)text[i]];
		d_img *img = &font->imgs.imgs[ii];
		d_draw_img(img, vec2f(pos.x + ox, pos.y));
		ox += img->width;
	}

}

void d_draw_line(vec2 p1, vec2 p2, color c) {

	if (c.a == 0) {
		return;
	}

	int dx = p2.x - p1.x;
	int dy = p2.y - p1.y;
	int adx = abs(dx);
	int ady = abs(dy);
	int eps = 0;
	int sx = dx > 0 ? 1 : -1;
	int sy = dy > 0 ? 1 : -1;

	if (adx > ady) {
		for(int x = p1.x, y = p1.y; sx < 0 ? x >= p2.x : x <= p2.x; x += sx) {
			d_put(vec2f(x, y), c);
			eps += ady;
			if ((eps << 1) >= adx) {
				y += sy;
				eps -= adx;
			}
		}
	} else {
		for(int x = p1.x, y = p1.y; sy < 0 ? y >= p2.y : y <= p2.y; y += sy) {
			d_put(vec2f(x, y), c);
			eps += adx;
			if ((eps << 1) >= ady) {
				x += sx;
				eps -= ady;
			}
		}
	}

}

void d_draw_mesh(const d_mesh *mesh) {
	// TODO
}

void d_draw_model(const d_model *model) {
	// TODO
}

void d_set_blend(d_blend b) {
	d_gfx.blend = b;
}

void d_drawon(d_img *img) {
	if (img) {
		d_gfx.cur_canvas = img;
	} else {
		d_gfx.cur_canvas = &d_gfx.def_canvas;
	}
}

d_img *d_canvas() {
	return d_gfx.cur_canvas;
}

