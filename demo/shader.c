// wengwengweng

#include <stdlib.h>
#include <dirty/dirty.h>

static const char* frag_code =
"uniform float u_time;"
"vec4 frag() {"
	"float angle = atan(v_uv.y, v_uv.x);"
	"float dis = length(v_uv);"
	"float time = u_time * 8.0;"
	"float c = sin(dis * 48.0 + time + angle);"
	"return vec4(c, c, c, 1.0) * u_color;"
"}"
;

d_shader shader;

static void frame() {

	if (d_key_pressed(D_KEY_ESC)) {
		d_quit();
	}

	d_use_shader(&shader);
	d_send_f("u_time", d_time());
	d_draw_rect(d_coord(D_TOP_LEFT), d_coord(D_BOT_RIGHT), WHITE);
	d_use_shader(NULL);

}

int main() {

	d_init("shader", 640, 480);

	shader = d_make_shader(NULL, frag_code);

	d_run(frame);

}

