app.run({
	title = "test",
	frame = function()
		if app.key_pressed("esc") then
			local mpos = app.mouse_pos()
			print(mpos)
			app.quit()
		end
	end,
})
