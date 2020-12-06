@vs vs
in vec2 a_pos;
in vec2 a_uv;

out vec2 v_uv;

void main() {
	gl_Position = vec4(a_pos, 0.0, 1.0);
	v_uv = a_uv;
}
@end

@fs fs
in vec2 v_uv;
out vec4 o_color;
uniform sampler2D u_tex;

void main() {
	o_color = texture(u_tex, v_uv);
}
@end

@program quad vs fs
