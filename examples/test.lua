-- wengwengweng

d_init("", 640, 480)

d_run(function()
	if d_key_pressed("esc") then
		d_quit()
	end
	if d_mouse_down("l") then
		print(d_time())
	end
end)

