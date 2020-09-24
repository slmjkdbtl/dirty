// wengwengweng

uniform float u_time;

vec4 frag() {

	float angle = atan(v_uv.y, v_uv.x);
	float dis = length(v_uv);
	float time = u_time * 8.0;
	float c = sin(dis * 48.0 + time + angle);

	return vec4(c, c, c, 1.0) * u_color;

}

