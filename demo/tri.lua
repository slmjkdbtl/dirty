-- wengwengweng

d_run({
	title = "hi",
	init = function()
		tex = d_load_tex("res/acid2.png")
		snd = d_load_sound("res/yo.ogg")
		pb = d_play(snd)
	end,
	frame = function()
		if d_key_pressed(d_key["esc"]) then
			d_quit()
		end
		if d_key_rpressed(d_key["space"]) then
			print("ouch")
		end
		d_draw_raw({
			{
				pos = vec3(0, 120, 0),
				normal = vec3(0, 0, 1),
				uv = vec2(0, 0),
				color = color(1, 0, 0, 1)
			},
			{
				pos = vec3(-160, -120, 0),
				normal = vec3(0, 0, 1),
				uv = vec2(0, 0),
				color = color(0, 1, 0, 1)
			},
			{
				pos = vec3(160, -120, 0),
				normal = vec3(0, 0, 1),
				uv = vec2(0, 0),
				color = color(0, 0, 1, 1)
			},
		}, { 0, 1, 2 })
		d_draw_tex(tex)
		d_draw_text("hi", 32)
	end,
})

