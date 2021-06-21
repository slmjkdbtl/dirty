// wengwengweng

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define CGLTF_IMPLEMENTATION
#include <cgltf.h>

#define D_IMPL
#define D_CPU
#define D_FS_ASYNC
#include <d_plat.h>
#include <d_fs.h>
#include <d_math.h>
#include <d_app.h>
#include <d_gfx.h>

#define WIDTH 240
#define HEIGHT 240
#define SCALE 2

#define FMT_MAX 256

static char *d_fmt(char *fmt, ...) {

	static char buf[FMT_MAX];
	va_list args;

	va_start(args, fmt);
	vsnprintf(buf, FMT_MAX, fmt, args);
	va_end(args);

	return buf;

}

typedef struct {
	vec3 pos;
	vec3 rot;
	vec3 scale;
	d_model model;
} model;

#define NUM_MODELS 4

model models[NUM_MODELS];
vec3 rot;
bool show_bbox;

void init() {

	d_gfx_init((d_gfx_desc) {
		.width = WIDTH,
		.height = HEIGHT,
		.clear_color = colori(0, 0, 0, 255),
	});

	d_fs_init((d_fs_desc) {0});

	models[0] = (model) {
		.pos = vec3f(60, 80, 0),
		.rot = vec3f(0.24, 0.48, 0),
		.scale = vec3f(6, -6, 6),
		.model = d_load_model("res/btfly.glb"),
	};

	models[1] = (model) {
		.pos = vec3f(160, 140, 0),
		.rot = vec3f(0.2, 0.3, 0.2),
		.scale = vec3f(1, -1, 1),
		.model = d_load_model("res/tv.glb"),
	};

	models[2] = (model) {
		.pos = vec3f(160, 60, 0),
		.rot = vec3f(0.5, -0.5, 0),
		.scale = vec3f(1, -1, 1),
		.model = d_load_model("res/flower.glb"),
	};

	models[3] = (model) {
		.pos = vec3f(40, 190, 0),
		.rot = vec3f(0.5, -0.5, 0),
		.scale = vec3f(1, -1, 1),
		.model = d_load_model("res/sprayer.glb"),
	};

}

void frame() {

	if (d_app_key_pressed(D_KEY_ESC)) {
		d_app_quit();
	}

	if (d_app_key_pressed(D_KEY_F)) {
		d_app_set_fullscreen(!d_app_fullscreen());
	}

	if (d_app_key_pressed(D_KEY_TAB)) {
		show_bbox = !show_bbox;
	}

	vec2 mdpos = d_gfx_mouse_dpos();

	if (d_app_mouse_down(D_MOUSE_LEFT)) {
		rot.x += mdpos.y / 100;
		rot.y += mdpos.x / 100;
	}

	d_gfx_clear();
	d_blit_bg();

	for (int i = 0; i < NUM_MODELS; i++) {
		model *m = &models[i];
		d_gfx_t_push();
		d_gfx_t_move3(m->pos);
		d_gfx_t_rot_y(m->rot.y + rot.y);
		d_gfx_t_rot_x(m->rot.x + rot.x);
		d_gfx_t_rot_z(m->rot.z);
		d_gfx_t_scale3(m->scale);
		d_gfx_t_move3(vec3_scale(m->model.center, -1));
		d_draw_model(&m->model);
		if (show_bbox) {
			d_draw_bbox(m->model.bbox, colorx(0x0000ffff));
		}
		d_gfx_t_pop();
	}

	d_gfx_present();

	d_app_set_title(d_fmt("%d", d_app_fps()));

}

int main() {
	d_app_run((d_app_desc) {
		.title = "3d",
		.init = init,
		.frame = frame,
		.width = WIDTH * SCALE,
		.height = HEIGHT * SCALE,
	});
}
