-- wengwengweng

d_run({
	title = "hi",
	init = function()
		print("oh hi")
		tex = d_load_tex("acid2.png")
		snd = d_load_sound("yo.ogg")
		pb = d_play(snd)
		print(snd.len)
	end,
	frame = function()
-- 		print(pb.time)
		if d_key_pressed("esc") then
			d_quit()
		end
		if d_key_rpressed("space") then
			print("ouch")
		end
		d_draw_tex(tex)
		d_draw_text("hi", 32)
	end,
})

