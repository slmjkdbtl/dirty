app.run({
	title = "test",
	frame = function()
		if app.key_pressed("esc") then
			app.quit()
		end
	end,
})
