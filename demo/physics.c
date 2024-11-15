#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define D_IMPL
#include <d_plat.h>
#include <d_fs.h>
#include <d_math.h>
#include <d_app.h>
#include <d_gfx.h>
#include <d_physics.h>

#define WIDTH 240
#define HEIGHT 240
#define SCALE 2
#define SPEED 100.0f
#define ROT_SPEED 3.0f

d_img img;
d_vec2 pos = V(20);
float rot = 0;

d_world world;
d_rect player_rect;
d_body* player_body;
d_body* rock1_body;
d_body* rock2_body;

void init(void) {

	d_gfx_init((d_gfx_desc) {
		.width = WIDTH,
		.height = HEIGHT,
		.clear_color = d_colori(0, 0, 0, 255),
	});

	img = d_img_load(d_res_path("res/wizard.png"));
	world = d_world_new();

	player_rect = (d_rect) { V(0), V(img.width, img.height) };
	player_body = d_world_add(&world, d_rect_to_poly(player_rect), NULL);
	rock1_body = d_world_add(&world, d_rect_to_poly((d_rect) { V(100), V(150) }), NULL);
	rock2_body = d_world_add(&world, d_rect_to_poly((d_rect) { V(40, 180), V(60, 200) }), NULL);

}

void frame(void) {

	if (d_app_key_pressed(D_KEY_ESC)) {
		d_app_quit();
	}

	if (d_app_key_pressed(D_KEY_F)) {
		d_app_set_fullscreen(!d_app_is_fullscreen());
	}

	float dt = d_app_dt();

	if (d_app_key_down(D_KEY_W)) {
		pos.y -= SPEED * dt;
	}

	if (d_app_key_down(D_KEY_S)) {
		pos.y += SPEED * dt;
	}

	if (d_app_key_down(D_KEY_A)) {
		pos.x -= SPEED * dt;
	}

	if (d_app_key_down(D_KEY_D)) {
		pos.x += SPEED * dt;
	}

	if (d_app_key_down(D_KEY_Q)) {
		rot -= ROT_SPEED * dt;
	}

	if (d_app_key_down(D_KEY_E)) {
		rot += ROT_SPEED * dt;
	}

	d_vec2 mpos = d_gfx_mouse_pos();

	d_transform_push();
	d_transform_pos(pos);
	d_transform_rot(rot);
	d_transform_pos(V(-img.width / 2.0f, -img.height / 2.0f));
	player_body->shape = d_rect_transform(player_rect, d_transform_get());
	d_transform_pop();

	d_gfx_clear();
	d_blit_bg();

	d_draw_poly(rock1_body->shape, C(0xffffff));
	d_draw_poly(rock2_body->shape, C(0xffffff));

	d_world_check_reset(&world);

	while (true) {
		d_world_res res = d_world_check_next(&world);
		if (!res.collided) break;
		if (res.body_a == player_body) {
			pos = d_vec2_add(pos, res.dis);
		} else if (res.body_b == player_body) {
			pos = d_vec2_sub(pos, res.dis);
		}
	}

	d_transform_push();
	d_transform_pos(pos);
	d_transform_rot(rot);
	d_transform_pos(d_vec2_scale(V(img.width, img.height), -0.5));
	d_draw_img(&img);
	d_draw_poly_lines(d_rect_to_poly(player_rect), C(0x0000ff));
	d_transform_pop();

	d_blit_circle(pos, 2, C(0xffff00));

	d_gfx_present();

}

int main(void) {
	d_app_run((d_app_desc) {
		.title = "move",
		.init = init,
		.frame = frame,
		.width = WIDTH * SCALE,
		.height = HEIGHT * SCALE,
	});
}
